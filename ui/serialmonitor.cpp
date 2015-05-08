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

#include "events.h"
#include "serialmonitor.h"
#include "strings.h"

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
    , len(0)
    , echo(true)
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
                buffer[len++] = uart.read();
                if (len == 1 && buffer[0] == '$')
                    echo = false;

                if (echo)
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
        uart.write_P(STR_PROMPT);
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
