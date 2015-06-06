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

#include "devices/ds3231.h"
#include "evse/settings.h"
#include "ui/customcharacters.h"
#include "ui/lcdstatesleeping.h"
#include "ui/strings.h"

#define BLINK_TIMEOUT 800

using devices::DS3231;
using evse::EepromSettings;
using evse::Settings;
using stream::LcdStream;

namespace ui
{

LcdStateSleeping::LcdStateSleeping(LcdStream &lcd)
    : LcdState(lcd)
    , blink_state(BLINK_TIMEOUT)
{
    Settings settings;
    EepromSettings::load(settings);
    sleep_mode = settings.sleep_mode;

    CustomCharacters::loadLargeDigits(lcd.getLCD());
}

bool LcdStateSleeping::draw()
{
    DS3231 &rtc = DS3231::get();

    // Display off mode
    if (sleep_mode == 1 || !rtc.isPresent())
    {
        lcd.setBacklight(devices::LCD16x2::OFF);
        return true;
    }

    // Display time mode

    lcd.setBacklight(devices::LCD16x2::GREEN);

    uint8_t buffer[10] = {0};
    rtc.readRaw(buffer, 8);

    const uint8_t hh = buffer[3];
    const uint8_t mm = buffer[2];

    CustomCharacters::largeDigit(lcd.getLCD(), hh >>  4,  1);
    CustomCharacters::largeDigit(lcd.getLCD(), hh & 0xF,  4);
    CustomCharacters::largeDigit(lcd.getLCD(), mm >>  4,  8);
    CustomCharacters::largeDigit(lcd.getLCD(), mm & 0xF, 11);

    const char o = blink_state.get() ? ' ' : 7;

    lcd.move(7, 1);
    lcd << static_cast<char>(o);
    lcd.move(7, 0);
    lcd << static_cast<char>(o);

    return true;
}

}
