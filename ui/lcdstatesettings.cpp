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

#include "system/timer.h"
#include "lcdstatesettings.h"
#include "strings.h"

#define SETTINGS_TIMEOUT    10000

using devices::LCD16x2;
using system::Timer;

namespace ui
{

LcdStateSettings::LcdStateSettings()
    : page(0)
    , option(0)
    , lastUpdate(0)
{
}

bool LcdStateSettings::draw(devices::LCD16x2 &lcd)
{
    if (timedOut())
        return false;

    lcd.setBacklight(LCD16x2::WHITE);

    lcd.move(0,0);
    lcd.write("SETTINGS ");
    lcd.write('0' + page);
    lcd.write(" -> ");
    lcd.write('0' + option);

    return true;
}

void LcdStateSettings::select()
{
    ++option;
    resetTimeout();
}

void LcdStateSettings::advance()
{
    ++page;
    option = 0;
    resetTimeout();
}

bool LcdStateSettings::timedOut()
{
    if (lastUpdate == 0)
        resetTimeout();
    return (Timer::millis() - lastUpdate) > SETTINGS_TIMEOUT;
}

void LcdStateSettings::resetTimeout()
{
    lastUpdate = Timer::millis();
}

}
