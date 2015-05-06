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

#include <util/delay.h>

#include "evse/state.h"
#include "lcdstateerror.h"
#include "strings.h"

using evse::State;

namespace ui
{

LcdStateError::LcdStateError(devices::LCD16x2 &lcd)
    : LcdState(lcd)
{
}

bool LcdStateError::draw()
{
    lcd.setBacklight(devices::LCD16x2::RED);

    lcd.move(0, 0);
    lcd.write_P(STR_ERROR_STATE);

    lcd.move(0, 1);

    switch (State::get().fault)
    {
        case State::FAULT_POST_GFCI:
            lcd.write_P(STR_FAULT_POST_GFCI);
            break;

        case State::FAULT_POST_RELAY:
            lcd.write_P(STR_FAULT_POST_RELAY);
            break;

        case State::FAULT_GFCI_TRIPPED:
            lcd.write_P(STR_FAULT_GFCI_TRIPPED);
            break;

        case State::FAULT_TEMPERATURE_CRITICAL:
            lcd.write_P(STR_FAULT_TEMPERATURE);
            break;

        case State::NOTHING_WRONG:
            // Then why are we here?!
            break;
    }
    return true;
}

}
