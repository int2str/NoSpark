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
#include "system/watchdog.h"
#include "ui/serialconsole.h"
#include "ui/strings.h"
#include "ui/strings_console.h"
#include "events.h"

using evse::ChargeMonitor;
using evse::EepromSettings;
using evse::Settings;
using evse::State;
using devices::DS3231;
using stream::PGM;

namespace
{
    uint8_t bcd_enc(const char msb_ch, const char lsb_ch)
    {
        return ((msb_ch - '0') << 4) | ((lsb_ch - '0') & 0x0F);
    }

    void write_time(stream::OutputStream &uart, DS3231 &rtc)
    {
        uint8_t buffer[8] = {0};
        rtc.readRaw(buffer, 8);

        uart << stream::PAD_BCD << buffer[3] << ':';
        uart << stream::PAD_BCD << buffer[2] << ':';
        uart << stream::PAD_BCD << buffer[1] << ' ';

        uart << stream::PAD_BCD << buffer[5] << '.';
        uart << stream::PAD_BCD << buffer[6] << '.';
        uart << "20" << stream::PAD_BCD << buffer[7];
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
        { STR_CMD_HELP,      false, &SerialConsole::commandHelp }
      , { STR_CMD_STATUS,    false, &SerialConsole::commandStatus }
      , { STR_CMD_VERSION,   false, &SerialConsole::commandVersion }
      , { STR_CMD_RESET,     false, &SerialConsole::commandReset }
      , { STR_CMD_SETCURRENT, true, &SerialConsole::commandSetCurrent }
      , { STR_CMD_SETTIME,    true, &SerialConsole::commandSetTime }
      , { STR_CMD_DEBUG,      true, &SerialConsole::commandDebug }
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
                uart << PGM << STR_EVENT;
                uart << PGM << STR_EVT_J1772_STATE;
                uart << static_cast<char>('A' - 1 + event.param) << CR;
            }
            break;

        case EVENT_CONTROLLER_STATE:
            if (event_debug)
            {
                uart << PGM << STR_EVENT;
                uart << PGM << STR_EVT_CONTROLLER_STATE;
                uart << static_cast<char>('0' + State::get().controller) << CR;
            }
            break;

        case EVENT_GFCI_TRIPPED:
            uart << PGM << STR_EVENT;
            uart << PGM << STR_EVT_GFCI_TRIPPED << CR;
            break;

        default:
        {
            if (event_debug)
            {
                char params[6] = {0};

                utoa(event.id, params, 10);
                uart << PGM << STR_EVENT;
                uart << PGM << STR_EVT_DEFAULT1;
                uart << params;

                utoa(event.param, params, 10);
                uart << PGM << STR_EVT_DEFAULT2;
                uart << params << CR;
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
        if (commands[i].hasParam)
        {
            const uint8_t cmd_len = strlen_P(commands[i].command);
            if (strncmp_P(buffer, commands[i].command, cmd_len) == 0)
            {
                (this->*commands[i].handler) (buffer, len);
                return true;
            }

        } else if (strcmp_P(buffer, commands[i].command) == 0) {
            (this->*commands[i].handler) (buffer, len);
            return true;
        }
    }

    uart << PGM << STR_HELP_UNKNOWN;
    uart << " (" << buffer << ")" << CR;

    return false;
}

void SerialConsole::commandHelp(const char *, const uint8_t)
{
    uart << PGM << STR_HELP_COMMANDS << CR;
    uart << PGM << STR_HELP_HELP << CR;
    uart << PGM << STR_HELP_RESET << CR;
    uart << PGM << STR_HELP_SETCURRENT << CR;
    uart << PGM << STR_HELP_SETTIME << CR;
    uart << PGM << STR_HELP_STATUS << CR;
    uart << PGM << STR_HELP_VERSION << CR;
    uart << PGM << STR_HELP_DEBUG << CR;
    uart << CR;
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
        uart << PGM << STR_ERR_PARAM << CR;
        return;
    }

    int amps = atoi(buffer + cmd_len);
    if (amps > 0)
    {
        State::get().max_amps_target = amps;
        saveMaxAmps(amps);
        event::Loop::post(event::Event(EVENT_MAX_AMPS_CHANGED, amps));

    } else {
        uart << PGM << STR_ERR_PARAM << CR;
    }
}

void SerialConsole::commandSetTime(const char *buffer, const uint8_t len)
{
    const uint8_t cmd_len = strlen_P(STR_CMD_SETTIME);

    if (len != cmd_len + 14 || buffer[cmd_len + 6] != ' ')
    {
        uart << PGM << STR_ERR_SETTIME_PARAM << CR;
        return;
    }

    const char *pp = buffer + cmd_len;
    uint8_t time_buffer[8] = {
        0 // <-- Register
      , bcd_enc(pp[4], pp[5])
      , bcd_enc(pp[2], pp[3])
      , bcd_enc(pp[0], pp[1])
      , 0
      , bcd_enc(pp[7], pp[8])
      , bcd_enc(pp[9], pp[10])
      , bcd_enc(pp[11], pp[12])
    };

    DS3231 &rtc = DS3231::get();
    rtc.writeRaw(time_buffer, 8);
}

void SerialConsole::commandDebug(const char *buffer, const uint8_t len)
{
    const uint8_t cmd_len = strlen_P(STR_CMD_DEBUG);

    if (len != cmd_len + 2)
    {
        uart << PGM << STR_ERR_PARAM << CR;
        return;
    }

    event_debug = buffer[cmd_len] != '0';
}

void SerialConsole::commandStatus(const char *, const uint8_t)
{
    DS3231 &rtc = DS3231::get();
    State &state = State::get();
    ChargeMonitor &cm = ChargeMonitor::get();

    uart << PGM << STR_STATUS_TIME;
    write_time(uart, rtc);
    uart << CR;

    uart << PGM << STR_STATUS_TEMP;
    uart << rtc.readTemp() << 'C' << CR;

    uart << PGM << (STR_STATUS_J1772);
    uart << static_cast<char>('A' - 1 + state.j1772) << CR;

    uart << PGM << (STR_STATUS_CHARGING);
    if (!cm.isCharging())
        uart << PGM << STR_OFF;
    else
        uart << cm.chargeCurrent() << "mA";
    uart << ' ' << (cm.chargeDuration() / 1000 / 60) << "min ";
    uart << cm.wattHours() << "Wh" << CR;

    uart << PGM << (STR_STATUS_READY);
    uart << static_cast<char>('0' + state.ready) << CR;

    uart << PGM << (STR_STATUS_MAX_CURRENT);
    uart << state.max_amps_limit << 'A' << '/';
    uart << state.max_amps_target << 'A' << CR;

    uart << CR;
}

void SerialConsole::commandVersion(const char *, const uint8_t)
{
    uart << PGM << STR_NOSPARK << CR;
    uart << PGM << STR_NOSPARK_BY << CR;
    uart << CR;
}

}
