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

#include "devices/lcd1602.h"
#include "evse/state.h"
#include "lcdstate.h"

namespace ui
{

class LcdStateRunning : public LcdState
{
public:
    LcdStateRunning(devices::LCD16x2 &lcd);
    bool draw();

private:
    enum RunningPages
    {
        PAGE_DEFAULT,
        PAGE_KWH_WEEK,
        PAGE_KWH_MONTH,
        PAGE_KWH_YEAR,
        PAGE_KWH_TOTAL,
        PAGE_MAX
    };

    void drawDefault();
    void drawKwhStats();
    void select();

    uint8_t page;
    uint8_t display_state;
    uint32_t last_change;
};

}
