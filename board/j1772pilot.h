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

#include <stdint.h>
#include "utils/cpp.h"

namespace board
{

// Uses timer1 for PWM, thus PIN 10 is automatically used,
// no need to abstract it.
class J1772Pilot
{
    J1772Pilot();

public:
    enum J1772Mode
    {
        HIGH
      , LOW
      , PWM
    };

    // Set to HIGH or LOW only, use pwmAmps to enable PWM
    static void set(const J1772Mode mode);
    static void pwmAmps(const uint8_t amps);

    static J1772Mode getMode();

private:
    static J1772Pilot& get();
    J1772Mode mode;

    DISALLOW_COPY_AND_ASSIGN(J1772Pilot);
};

}
