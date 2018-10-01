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

#include "board/j1772pilot.h"
#include "utils/cpp.h"
#include "settings.h"

namespace nospark
{
namespace evse
{

class State
{
    State();

public:
    enum ControllerState
    {
        BOOTUP
      , POST
      , RUNNING
      , FAULT
    };

    enum ControllerFault
    {
        NOTHING_WRONG
      , FAULT_POST_GFCI
      , FAULT_STUCK_RELAY
      , FAULT_GROUND_FAULT
      , FAULT_GFCI_TRIPPED
      , FAULT_TEMPERATURE_CRITICAL
    };

    enum ReadyState
    {
        READY
      , SCHEDULED
      , KWH_LIMIT
      , MANUAL_OVERRIDE
    };

    static State& get();

    ControllerState controller;
    ControllerFault fault;
    ReadyState ready;
    board::J1772Pilot::State j1772;

    // Desired maximum charge current
    uint8_t max_amps_target;

    // Current limit in effect
    // This value will be <= max_amps_target and is
    // affected by temperature monitoring etc.
    uint8_t max_amps_limit;

private:
    DISALLOW_COPY_AND_ASSIGN(State);
};

}
}
