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

#include "devices/lcd1602.h"
#include "lcdstate.h"

namespace ui
{

class LcdStateSettings : public LcdState
{
public:
    LcdStateSettings(devices::LCD16x2 &lcd);

    bool draw();

    void pageSetClock();
    void pageSetCurrent();
    void pageReset();
    void pageExit();

    // Forwarded keyboard events
    virtual void select();
    virtual void advance();

private:
    bool timedOut();
    void resetTimeout();

    uint8_t page;
    uint8_t option;
    bool adjusting;

    uint32_t lastAction;
    uint32_t lastUpdate;
    uint8_t uiState;
};

}
