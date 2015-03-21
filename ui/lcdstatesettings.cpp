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
#include <stdlib.h>

#include "system/timer.h"
#include "system/watchdog.h"
#include "customcharacters.h"
#include "lcdstatesettings.h"
#include "strings.h"

#define SETTINGS_PAGES          4
#define SETTINGS_TIMEOUT    10000
#define ADJUST_TIMEOUT       5000

using devices::LCD16x2;
using system::Timer;
using system::Watchdog;

namespace ui
{

LcdStateSettings::LcdStateSettings(devices::LCD16x2 &lcd)
    : LcdState(lcd)
    , page(0)
    , option(0)
    , adjusting(false)
    , lastAction(0)
    , lastUpdate(0)
    , uiState(0)
{
}

bool LcdStateSettings::draw()
{
    if (timedOut())
        return false;

    lcd.setBacklight(LCD16x2::WHITE);

    switch (page)
    {
        case 0:
            pageSetClock();
            break;

        case 1:
            pageSetCurrent();
            break;

        case 2:
            pageReset();
            break;

        case 3:
            pageExit();
            if (adjusting)
                return false;
            break;
    }

    return true;
}

void LcdStateSettings::pageSetClock()
{
    lcd.move(0,0);
    lcd.write(CUSTOM_CHAR_CLOCK);
    lcd.write_P(STR_SET_CLOCK);
}

void LcdStateSettings::pageSetCurrent()
{
    uint8_t current = 24 + option;
    char buffer[4] = {0};
    utoa(current, buffer, 10);

    lcd.move(0, 0);
    lcd.write(CUSTOM_CHAR_BOLT);
    lcd.write_P(STR_SET_CURRENT);

    lcd.move(2, 1);
    if (!adjusting || uiState)
    {
        lcd.write(buffer);
        lcd.write(" Ampere ");
    } else {
        lcd.write("             ");
    }

    if (Timer::millis() - lastUpdate > 500)
    {
        uiState = !uiState;
        lastUpdate = Timer::millis();
    }
}

void LcdStateSettings::pageReset()
{
    lcd.move(0, 0);
    lcd.write('!');
    lcd.write_P(STR_SET_RESET);
    if (adjusting)
        Watchdog::forceRestart();
}

void LcdStateSettings::pageExit()
{
    lcd.move(0, 0);
    lcd.write(0x7F); // <- back arrow
    lcd.write_P(STR_SET_EXIT);
}

void LcdStateSettings::select()
{
    if (!adjusting)
    {
        lcd.clear();
        if (++page == SETTINGS_PAGES)
            page = 0;
        option = 0;
    } else {
        ++option;
    }

    resetTimeout();
}

void LcdStateSettings::advance()
{
    uiState = 0;
    adjusting = !adjusting;
    resetTimeout();
}

bool LcdStateSettings::timedOut()
{
    if (lastAction == 0)
        resetTimeout();
    return (Timer::millis() - lastAction) > SETTINGS_TIMEOUT;
}

void LcdStateSettings::resetTimeout()
{
    lastAction = Timer::millis();
}

}
