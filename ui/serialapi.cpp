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
#include "evse/settings.h"
#include "evse/state.h"
#include "system/watchdog.h"
#include "serialapi.h"
#include "events.h"
#include "strings.h"

using evse::EepromSettings;
using evse::Settings;
using evse::State;
using devices::DS3231;
using serial::Usart;

namespace
{
    enum ErrorCodes
    {
        OK
      , UNKNOWN_COMMAND
      , INVALID_PARAMETER
    };

    // Save on event change somehow? Simplify code?
    // Un-dupe? :)
    void saveMaxAmps(const uint8_t max_amps)
    {
        Settings settings;
        EepromSettings::load(settings);
        settings.max_current = max_amps;
        EepromSettings::save(settings);
    }

    uint8_t paramU8(const char *buffer, const char param)
    {
        bool found = false;
        uint8_t val = 0xFF;

        while (*buffer != 0)
        {
            const char ch = *buffer++;
            if (found)
            {
                if (ch < '0' || ch > '9')
                    break;
                val *= 10;
                val += ch - '0';
            }

            if (ch == param)
            {
                val = 0;
                found = true;
            }
        }

        return val;
    }
}

namespace ui
{

SerialApi::SerialApi(serial::Usart &uart)
    : uart(uart)
{
}

void SerialApi::onEvent(const event::Event&)
{
}

bool SerialApi::handleCommand(const char *buffer, const uint8_t)
{
    const uint8_t e = paramU8(buffer, 'E');
    uint8_t err = OK;

    switch (e)
    {
        case 1:
            err = commandSetCurrent(buffer);
            break;

        default:
            err = UNKNOWN_COMMAND;
            break;
    }

    if (err == 0)
    {
        uart.writeln("OK");
    } else {
        char buffer[] = "ERR 0";
        buffer[4] += err;
        uart.writeln(buffer);
    }

    return true;
}

uint8_t SerialApi::commandSetCurrent(const char *buffer)
{
    const uint8_t amps = paramU8(buffer, 'A');
    if (amps == 0 || amps == 0xFF)
        return INVALID_PARAMETER;

    State::get().max_amps_target = amps;
    saveMaxAmps(amps);
    event::Loop::post(event::Event(EVENT_MAX_AMPS_CHANGED, amps));

    return OK;
}

}
