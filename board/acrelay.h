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
#include <stdint.h>

#include "board/pin.h"
#include "utils/cpp.h"

namespace board
{

// Relay monitoring and control
class ACRelay
{
public:
    enum RelayState
    {
        UNKNOWN
      , OK
      , STUCK_RELAY
      , GROUND_FAULT
    };

    ACRelay();

    void setState(const bool enable);

    // This will sample the relay sense pins and
    // return an appropriate status depending on
    // whether the relays are currently enabled or
    // not and whether power is present or not.
    RelayState checkStatus();

private:
    RelayState state;
    bool enabled;
    uint32_t last_change;

    board::Pin pinACRelay;
    board::Pin pinDCRelay1;
    board::Pin pinDCRelay2;

    // The two sense pins are hooked together, so they'll
    // always read the same on the v3 units.
    board::Pin pinSense1;
    board::Pin pinSense2;

    uint8_t getActive() const;

    DISALLOW_COPY_AND_ASSIGN(ACRelay);
};

}
