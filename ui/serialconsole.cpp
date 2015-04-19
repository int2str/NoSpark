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
#include "evse/state.h"
#include "system/timer.h"
#include "system/watchdog.h"
#include "serialconsole.h"
#include "events.h"
#include "strings.h"

using evse::State;
using serial::Usart;
using devices::DS3231;

namespace
{
    uint8_t bcd_enc(const char msb_ch, const char lsb_ch)
    {
        return ((msb_ch - '0') << 4) | ((lsb_ch - '0') & 0x0F);
    }

    void write_digits(Usart& uart, const uint8_t d)
    {
        uart.write('0' + (d >> 4));
        uart.write('0' + (d & 0x0F));
    }

    void write_decimal(Usart& uart, const uint8_t dec)
    {
        if (dec > 10)
            uart.write('0' + (dec / 10));
        uart.write('0' + (dec % 10));
    }

    void write_temp(Usart& uart, const uint8_t temp)
    {
        write_decimal(uart, temp);
        uart.write('C');
    }

    void write_time(Usart& uart, DS3231 &rtc)
    {
        uint8_t buffer[7] = {0};
        rtc.readRaw(buffer, 7);

        write_digits(uart, buffer[2]);
        uart.write(':');
        write_digits(uart, buffer[1]);
        uart.write(':');
        write_digits(uart, buffer[0]);
        uart.write(' ');

        write_digits(uart, buffer[4]);
        uart.write('.');
        write_digits(uart, buffer[5]);
        uart.write(".20");
        write_digits(uart, buffer[6]);
    }
}

namespace ui
{

SerialConsole& SerialConsole::init()
{
    static SerialConsole console;
    return console;
}

SerialConsole::SerialConsole()
    : uart(serial::Usart::get())
    , state(CONSOLE_STARTUP)
    , event_debug(false) // TODO: Read from EEPROM?
    , len(0)
    , commands({
        { STR_CMD_HELP,      false, &SerialConsole::commandHelp }
      , { STR_CMD_STATUS,    false, &SerialConsole::commandStatus }
      , { STR_CMD_RESET,     false, &SerialConsole::commandReset }
      , { STR_CMD_SETCURRENT, true, &SerialConsole::commandSetCurrent }
      , { STR_CMD_SETTIME,    true, &SerialConsole::commandSetTime }
      , { STR_CMD_DEBUG,      true, &SerialConsole::commandDebug }
    })
{
}

void SerialConsole::update()
{
    switch (state)
    {
        case CONSOLE_STARTUP:
            uart.write("\r\r");
            uart.writeln_P(STR_NOSPARK);
            uart.writeln_P(STR_NOSPARK_BY);
            uart.write(CR);
            state = CONSOLE_READY;
            break;

        case CONSOLE_READY:
            uart.write_P(STR_PROMPT);
            len = 0;
            state = CONSOLE_ACCUMULATING;
            break;

        case CONSOLE_ACCUMULATING:
            while (uart.avail())
            {
                buffer[len++] = uart.read();
                uart.write(buffer[len-1]); // <-- echo

                if (len == CONSOLE_BUFFER-1 || buffer[len-1] == CR)
                {
                    buffer[len-1] = 0;
                    state = CONSOLE_COMMAND;
                    break;
                }
            }
            break;

        case CONSOLE_COMMAND:
            state = handleCommand();
            break;
    }
}

void SerialConsole::onEvent(const event::Event& event)
{
    switch (event.id)
    {
        case EVENT_UPDATE:
            update();
            break;

        case EVENT_J1772_STATE:
            if (event_debug)
            {
                uart.write_P(STR_EVENT);
                uart.write_P(STR_EVT_J1772_STATE);
                uart.write('A' - 1 + event.param);
                uart.write(CR);
            }
            break;

        case EVENT_CONTROLLER_STATE:
            if (event_debug)
            {
                uart.write_P(STR_EVENT);
                uart.write_P(STR_EVT_CONTROLLER_STATE);
                uart.write('0' + State::get().controller);
                uart.write(CR);
            }
            break;

        case EVENT_GFCI_TRIPPED:
            uart.write_P(STR_EVENT);
            uart.writeln_P(STR_EVT_GFCI_TRIPPED);
            break;

        default:
        {
            if (event_debug)
            {
                char params[6] = {0};

                utoa(event.id, params, 10);
                uart.write_P(STR_EVENT);
                uart.write_P(STR_EVT_DEFAULT1);
                uart.write(params);

                utoa(event.param, params, 10);
                uart.write_P(STR_EVT_DEFAULT2);
                uart.writeln(params);
            }
            break;
        }
    }
}

SerialConsole::SerialState SerialConsole::handleCommand()
{
    if (buffer[0] == 0)
        return CONSOLE_READY;

    for (uint8_t i = 0; i != MAX_COMMANDS; ++i)
    {
        if (commands[i].hasParam)
        {
            const uint8_t cmd_len = strlen_P(commands[i].command);
            if (strncmp_P(buffer, commands[i].command, cmd_len) == 0)
            {
                (this->*commands[i].handler) ();
                return CONSOLE_READY;
            }

        } else if (strcmp_P(buffer, commands[i].command) == 0) {
            (this->*commands[i].handler) ();
            return CONSOLE_READY;
        }
    }

    uart.writeln_P(STR_HELP_UNKNOWN);

    return CONSOLE_READY;
}

void SerialConsole::commandHelp()
{
    uart.writeln_P(STR_HELP_COMMANDS);
    uart.writeln_P(STR_HELP_HELP);
    uart.writeln_P(STR_HELP_RESET);
    uart.writeln_P(STR_HELP_SETCURRENT);
    uart.writeln_P(STR_HELP_SETTIME);
    uart.writeln_P(STR_HELP_STATUS);
    uart.writeln_P(STR_HELP_DEBUG);
    uart.write(CR);
}

void SerialConsole::commandReset()
{
    system::Watchdog::forceRestart();
}

void SerialConsole::commandSetCurrent()
{
    const uint8_t cmd_len = strlen_P(STR_CMD_SETCURRENT);

    if (len < cmd_len + 2 || len > cmd_len + 3)
    {
        uart.writeln_P(STR_ERR_PARAM);
        return;
    }

    int amps = atoi(buffer + cmd_len);
    if (amps > 0)
    {
        State::get().max_amps_target = amps;
        event::Loop::post(event::Event(EVENT_MAX_AMPS_CHANGED, amps));

    } else {
        uart.writeln_P(STR_ERR_PARAM);
    }
}

void SerialConsole::commandSetTime()
{
    const uint8_t cmd_len = strlen_P(STR_CMD_SETTIME);

    if (len != cmd_len + 14 || buffer[cmd_len + 6] != ' ')
    {
        uart.writeln_P(STR_ERR_SETTIME_PARAM);
        return;
    }

    char *pp = buffer + cmd_len;
    uint8_t buffer[8] = {
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
    rtc.writeRaw(buffer, 8);
}

void SerialConsole::commandDebug()
{
    const uint8_t cmd_len = strlen_P(STR_CMD_DEBUG);

    if (len != cmd_len + 2)
    {
        uart.writeln_P(STR_ERR_PARAM);
        return;
    }

    event_debug = buffer[cmd_len] != '0';
}

void SerialConsole::commandStatus()
{
    DS3231 &rtc = DS3231::get();
    State &state = State::get();

    uart.write_P(STR_STATUS_TIME);
    write_time(uart, rtc);
    uart.write(CR);

    uart.write_P(STR_STATUS_TEMP);
    write_temp(uart, rtc.readTemp());
    uart.write(CR);

    uart.write_P(STR_STATUS_J1772);
    uart.write('A' - 1 + state.j1772);
    uart.write(CR);

    uart.write_P(STR_STATUS_MAX_CURRENT);
    write_decimal(uart, state.max_amps_limit);
    uart.write('A');
    uart.write('/');
    write_decimal(uart, state.max_amps_target);
    uart.write('A');
    uart.write(CR);

    uart.write(CR);
}

}
