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

#include "system/watchdog.h"
#include "lcdstatebootup.h"
#include "strings.h"

#define FADE_DELAY_MS  25
#define LCD_COLUMNS    16

namespace
{
    void introFade(stream::LcdStream &lcd, const char ch)
    {
        for (uint8_t x = 0; x != LCD_COLUMNS; ++x)
        {
            lcd.move(x, 0);
            lcd << ch;
            lcd.move(LCD_COLUMNS-x-1, 1);
            lcd << ch;
            _delay_ms(FADE_DELAY_MS);
        }
    }
}

namespace ui
{

LcdStateBootup::LcdStateBootup(stream::LcdStream &lcd)
    : LcdState(lcd)
    , intro_shown(false)
{
}

void LcdStateBootup::screen(const char *line1, const char *line2)
{
    lcd.move(0,0);
    lcd << stream::PGM << line1;
    lcd.move(0,1);
    lcd << stream::PGM << line2;

    _delay_ms(1200);
    system::Watchdog::reset();

    introFade(lcd, static_cast<char>(0xFF));
    introFade(lcd, ' ');
}

bool LcdStateBootup::draw()
{
    if (intro_shown)
        return false;

    lcd.setBacklight(devices::LCD16x2::WHITE);

    screen(STR_OPENEVSE, STR_OPENEVSE_BY);
    screen(STR_NOSPARK, STR_NOSPARK_BY);
    screen(STR_EXTRA, STR_EXTRA_BY);

    intro_shown = true;
    return true;
}

}
