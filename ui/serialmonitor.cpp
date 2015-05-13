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

#include "ui/serialmonitor.h"
#include "ui/strings.h"
#include "ui/strings_console.h"
#include "events.h"

using serial::Usart;

namespace ui
{

SerialMonitor& SerialMonitor::init()
{
    static SerialMonitor monitor;
    return monitor;
}

SerialMonitor::SerialMonitor()
    : uart(serial::Usart::get())
    , api(uart)
    , console(uart)
    , state(CONSOLE_STARTUP)
    , echo(true)
    , len(0)
{
}

void SerialMonitor::update()
{
    switch (state)
    {
        case CONSOLE_STARTUP:
            // Good place to set BT name etc...
            state = CONSOLE_ACCUMULATING;
            break;

        case CONSOLE_ACCUMULATING:
            while (uart.avail())
            {
                uart >> buffer[len++];

                if (len == 1 && buffer[0] == '$')
                    echo = false;

                if (echo)
                    uart << static_cast<char>(buffer[len-1]); // <-- echo

                if (len == CONSOLE_BUFFER-1 || buffer[len-1] == CR)
                {
                    buffer[len-1] = 0;
                    state = CONSOLE_COMMAND;
                    break;
                }
            }
            break;

        case CONSOLE_COMMAND:
            handleCommand();
            len = 0;
            echo = true;
            state = CONSOLE_ACCUMULATING;
            break;
    }
}

void SerialMonitor::handleCommand()
{
    if (len > 2 && buffer[0] == '$')
    {
        api.handleCommand(buffer+1, len-1);
    } else {
        console.handleCommand(buffer, len);
        uart << stream::PGM << STR_PROMPT;
    }
}

void SerialMonitor::onEvent(const event::Event& event)
{
    switch (event.id)
    {
        case EVENT_UPDATE:
            update();
            break;

        default:
            console.onEvent(event);
            break;
    }
}

}
