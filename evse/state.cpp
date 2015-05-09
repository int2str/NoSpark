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

#include "evse/settings.h"
#include "utils/eeprom.h"
#include "state.h"

namespace evse
{

State& State::get()
{
    static State state;
    return state;
}

State::State()
    : controller(BOOTUP)
    , fault(NOTHING_WRONG)
    , charge(READY)
    , j1772(board::J1772Pilot::UNKNOWN)
{
    Settings settings;
    EepromSettings::load(settings);
    max_amps_target = settings.max_current;
    max_amps_limit = max_amps_target;
}

}
