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

#include "board/acrelay.h"
#include "board/gfci.h"
#include "board/j1772status.h"
#include "evse/state.h"
#include "event/handler.h"
#include "utils/cpp.h"

namespace evse
{

class Controller : public event::Handler
{
    Controller();
    ~Controller();

public:
    static Controller& init();

private:
    void onEvent(const event::Event &event);

    void update();
    void updateRunning();

    void enableCharge(const bool enable);
    void updateChargeCurrent(const bool enablePwm = false);

    void setFault(const State::ControllerFault fault);

    // This toggles the pilot pin, so this function
    // should never be called during normal operation.
    bool checkEVPresent();

    board::ACRelay& acRelay;
    board::GFCI& gfci;
    board::J1772Status& j1772Status;

    uint32_t lastStateChange;

    DISALLOW_COPY_AND_ASSIGN(Controller);
};

}
