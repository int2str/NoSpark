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

#pragma once

#include <stdbool.h>

#include "board/pin.h"
#include "utils/cpp.h"

namespace board
{

// AC relay control for OpenEVSE v3
class ACRelay
{
public:
    ACRelay();

    void enable();
    void disable();
    bool isActive() const;
    void selfTest(const bool evPresent);

private:
    board::Pin pinRelay;

    // The two sense pins are hooked together, so they'll
    // always read the same. Just in case, we'll enable
    // the pullup on both though so that the 2nd pin doesn't
    // actually pull down the first one.
    board::Pin pinSense;
    board::Pin pinSenseLegacy;

    DISALLOW_COPY_AND_ASSIGN(ACRelay);
};

}
