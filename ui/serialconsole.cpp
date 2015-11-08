// NoSpark - OpenEVSE charger firmware
// Copyright (C) 2015 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>.

#include <avr/pgmspace.h>
#include <stdlib.h>

#include "devices/ds3231.h"
#include "event/loop.h"
#include "evse/chargemonitor.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "stream/time.h"
#include "system/watchdog.h"
#include "ui/serialconsole.h"
#include "ui/strings.h"
#include "ui/strings_console.h"
#include "events.h"

#define AMPS_MIN  6
#define AMPS_MAX 80

using evse::ChargeMonitor;
using evse::EepromSettings;
using evse::Settings;
using evse::State;
using devices::DS3231;
using devices::TM;
using stream::PGM;

namespace
{
    uint8_t asc_bin(const char *s)
    {
        return (s[0] - '0') * 10 + (s[1] - '0');
    }

    void write_help(stream::OutputStream &out, const char *cmd, const char *help)
    {
        out << ' ' << PGM << cmd;
        uint8_t len = strlen_P(cmd);
        while (len++ < 12)
            out << ' ';
        out << PGM << help << EOL;
    }

    void write_energy_stat(stream::OutputStream &out, const char *label, const uint8_t currency, const uint32_t wh, uint32_t cost)
    {
        char buffer[10] = {0};
        ltoa(wh / 1000, buffer, 10);
        out << ' ' << PGM << label << ": " << buffer << " kWh";

        if (cost != 0)
        {
            const char currencies[3] = {'$', 'E', 'Y'};

            char buffer[10] = {0};
            ltoa(cost / 100, buffer, 10);
            out << " / " << static_cast<char>(currencies[currency]) << buffer << ".";

            cost %= 100;
            out << static_cast<char>('0' + cost / 10);
            out << static_cast<char>('0' + cost % 10);
        }
        out << EOL;
    }

    // TODO: Duplicate code is duplicate...
    // Save on event change somehow? Simplify code?
    void saveMaxAmps(const uint8_t max_amps)
    {
        Settings settings;
        EepromSettings::load(settings);
        settings.max_current = max_amps;
        EepromSettings::save(settings);
    }
}

namespace ui
{

SerialConsole::SerialConsole(stream::UartStream &uart)
    : uart(uart)
    , event_debug(false) // TODO: Read from EEPROM?
    , commands({
        { STR_CMD_HELP,       &SerialConsole::commandHelp }
      , { STR_CMD_STATUS,     &SerialConsole::commandStatus }
      , { STR_CMD_ENERGY,     &SerialConsole::commandEnergy }
      , { STR_CMD_VERSION,    &SerialConsole::commandVersion }
      , { STR_CMD_RESET,      &SerialConsole::commandReset }
      , { STR_CMD_SETCURRENT, &SerialConsole::commandSetCurrent }
      , { STR_CMD_SETLIMIT  , &SerialConsole::commandSetLimit }
      , { STR_CMD_SETTIME,    &SerialConsole::commandSetTime }
      , { STR_CMD_DEBUG,      &SerialConsole::commandDebug }
    })
{
}

void SerialConsole::onEvent(const event::Event& event)
{
    switch (event.id)
    {
        case EVENT_UPDATE:
            break;

        case EVENT_J1772_STATE:
            if (event_debug)
            {
                uart << PGM << STR_EVENT
                  << PGM << STR_EVT_J1772_STATE
                  << static_cast<char>('A' - 1 + event.param) << EOL;
            }
            break;

        case EVENT_CONTROLLER_STATE:
            if (event_debug)
            {
                uart << PGM << STR_EVENT
                  << PGM << STR_EVT_CONTROLLER_STATE
                  << static_cast<char>('0' + State::get().controller) << EOL;
            }
            break;

        case EVENT_GFCI_TRIPPED:
            uart << PGM << STR_EVENT
              << PGM << STR_EVT_GFCI_TRIPPED << EOL;
            break;

        default:
        {
            if (event_debug)
            {
                char params[6] = {0};

                utoa(event.id, params, 10);
                uart << PGM << STR_EVENT
                  << PGM << STR_EVT_DEFAULT1
                  << params;

                utoa(event.param, params, 10);
                uart << PGM << STR_EVT_DEFAULT2
                  << params << EOL;
            }
            break;
        }
    }
}

bool SerialConsole::handleCommand(const char *buffer, const uint8_t len)
{
    if (buffer[0] == 0)
        return false;

    for (uint8_t i = 0; i != MAX_COMMANDS; ++i)
    {
        const uint8_t cmd_len = strlen_P(commands[i].command);
        if (strncmp_P(buffer, commands[i].command, cmd_len) == 0)
        {
            if (len > cmd_len && buffer[cmd_len] != ' ')
                continue;

            (this->*commands[i].handler) (buffer, len);
            return true;
        }
    }

    uart << PGM << STR_HELP_UNKNOWN << EOL;

    return false;
}

void SerialConsole::commandHelp(const char *, const uint8_t)
{
    uart << PGM << STR_HELP_COMMANDS << EOL;
    write_help(uart, STR_CMD_HELP, STR_HELP_HELP);
    write_help(uart, STR_CMD_ENERGY, STR_HELP_ENERGY);
    write_help(uart, STR_CMD_RESET, STR_HELP_RESET);
    write_help(uart, STR_CMD_SETCURRENT, STR_HELP_SETCURRENT);
    write_help(uart, STR_CMD_SETLIMIT, STR_HELP_SETLIMIT);
    write_help(uart, STR_CMD_SETTIME, STR_HELP_SETTIME);
    write_help(uart, STR_CMD_STATUS, STR_HELP_STATUS);
    write_help(uart, STR_CMD_VERSION, STR_HELP_VERSION);
    write_help(uart, STR_CMD_DEBUG, STR_HELP_DEBUG);
    uart << EOL;
}

void SerialConsole::commandReset(const char *, const uint8_t)
{
    system::Watchdog::forceRestart();
}

void SerialConsole::commandSetCurrent(const char *buffer, const uint8_t len)
{
    const uint8_t cmd_len = strlen_P(STR_CMD_SETCURRENT);

    if (len < cmd_len + 2 || len > cmd_len + 3)
    {
        uart << PGM << STR_ERR_PARAM << EOL;
        return;
    }

    const int amps = atoi(buffer + cmd_len);
    if (amps >= AMPS_MIN && amps <= AMPS_MAX)
    {
        State::get().max_amps_target = amps;
        saveMaxAmps(amps);
        event::Loop::post(event::Event(EVENT_MAX_AMPS_CHANGED, amps));

    } else {
        uart << PGM << STR_ERR_PARAM << EOL;
    }
}

void SerialConsole::commandSetLimit(const char *buffer, const uint8_t len)
{
    const uint8_t cmd_len = strlen_P(STR_CMD_SETLIMIT);

    if (len < cmd_len + 2 || len > cmd_len + 3)
    {
        uart << PGM << STR_ERR_PARAM << EOL;
        return;
    }

    const int kwh = atoi(buffer + cmd_len);
    if (kwh >= 0)
    {
        Settings settings;
        EepromSettings::load(settings);
        settings.kwh_limit = kwh;
        EepromSettings::save(settings);

    } else {
        uart << PGM << STR_ERR_PARAM << EOL;
    }
}

void SerialConsole::commandSetTime(const char *buffer, const uint8_t len)
{
    const uint8_t cmd_len = strlen_P(STR_CMD_SETTIME);

    if (len != cmd_len + 14 || buffer[cmd_len + 6] != ' ')
    {
        uart << PGM << STR_ERR_SETTIME_PARAM << EOL;
        return;
    }

    DS3231 &rtc = DS3231::get();
    const char *pp = buffer + cmd_len;
    TM t;

    rtc.read(t);

    t.hour = asc_bin(pp);
    t.minute = asc_bin(pp + 2);
    t.second = asc_bin(pp + 4);

    t.day = asc_bin(pp + 7);
    t.month = asc_bin(pp + 9);
    t.year = asc_bin(pp + 11);

    t.setWeekday();

    rtc.write(t);
}

void SerialConsole::commandEnergy(const char*, const uint8_t)
{
    Settings settings;
    EepromSettings::load(settings);

    uart << PGM << STR_STATS_KWH << EOL;
    write_energy_stat(uart, STR_STATS_WEEK, settings.kwh_currency, settings.wh_week, settings.cost_week);
    write_energy_stat(uart, STR_STATS_MONTH, settings.kwh_currency, settings.wh_month, settings.cost_month);
    write_energy_stat(uart, STR_STATS_YEAR, settings.kwh_currency, settings.wh_year, settings.cost_year);
    write_energy_stat(uart, STR_STATS_TOTAL, settings.kwh_currency, settings.wh_total, settings.cost_total);
    uart << EOL;
}

void SerialConsole::commandDebug(const char *buffer, const uint8_t len)
{
    const uint8_t cmd_len = strlen_P(STR_CMD_DEBUG);

    if (len != cmd_len + 2)
    {
        uart << PGM << STR_ERR_PARAM << EOL;
        return;
    }

    event_debug = buffer[cmd_len] != '0';
}

void SerialConsole::commandStatus(const char *, const uint8_t)
{
    State &state = State::get();
    ChargeMonitor &cm = ChargeMonitor::get();

    Settings settings;
    EepromSettings::load(settings);

    DS3231 &rtc = DS3231::get();
    if (rtc.isPresent())
    {
        TM t;
        rtc.read(t);

        uart << PGM << STR_STATUS_TIME
          << stream::Time(t.hour, t.minute) << ' '
          << t.day << '.' << t.month << '.' << "20" << t.year
          << EOL;

        uart << PGM << STR_STATUS_TEMP
          << rtc.readTemp() << 'C' << EOL;
    }

    uart << PGM << (STR_STATUS_J1772)
      << static_cast<char>('A' - 1 + state.j1772) << EOL;

    uart << PGM << STR_STATUS_CHARGING;
    if (!cm.isCharging())
        uart << PGM << STR_OFF;
    else
        uart << cm.chargeCurrent() << "mA";

    uart << ' ' << (cm.chargeDuration() / 1000 / 60) << "min "
      << cm.wattHours() << "Wh" << EOL;

    uart << PGM << (STR_STATUS_READY)
      << static_cast<char>('0' + state.ready) << EOL;

    uart << PGM << STR_STATUS_MAX_CURRENT
      << state.max_amps_limit << 'A' << '/'
      << state.max_amps_target << 'A' << EOL;

    if (settings.kwh_limit > 0)
        uart << PGM << STR_STATUS_LIMIT << settings.kwh_limit << "kWh" << EOL;

    uart << EOL;
}

void SerialConsole::commandVersion(const char *, const uint8_t)
{
    uart << PGM << STR_NOSPARK << EOL
      << PGM << STR_NOSPARK_BY << EOL
      << EOL;
}

}
