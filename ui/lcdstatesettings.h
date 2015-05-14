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

#include "evse/settings.h"
#include "stream/lcdstream.h"
#include "ui/lcdstate.h"
#include "ui/timedflipflop.h"

#define SETTINGS_PAGES 9

namespace ui
{

class LcdStateSettings;
typedef bool (LcdStateSettings::*PageHandler) ();

class LcdStateSettings : public LcdState
{
public:
    LcdStateSettings(stream::LcdStream &lcd);

    bool draw() override;

    bool pageSetTime();
    bool pageSetDate();
    bool pageSetCurrent();
    bool pageChargeTimer();
    bool pageKwhLimit();
    bool pageKwhCost();
    bool pageSleepmode();
    bool pageReset();
    bool pageExit();

    // Forwarded keyboard events
    virtual void select() override;
    virtual void advance() override;

private:
    bool timedOut();
    void resetTimeout();

    void load();
    void save();

    evse::Settings settings;

    uint8_t page;
    uint8_t option;
    uint8_t value;

    uint32_t last_action;
    TimedFlipFlop blink_state;

    PageHandler pageHandlers[SETTINGS_PAGES];
};

}
