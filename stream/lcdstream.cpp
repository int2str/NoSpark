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

#include "stream/lcdstream.h"

using devices::LCD16x2;

namespace stream
{

LcdStream::LcdStream(LCD16x2 &lcd)
  : lcd(lcd)
{
}

LCD16x2& LcdStream::getLCD() const
{
    return lcd;
}

void LcdStream::clear()
{
    lcd.clear();
}

void LcdStream::move(const uint8_t x, const uint8_t y)
{
    lcd.move(x, y);
}

void LcdStream::setBacklight(const LCD16x2::Backlight color)
{
    lcd.setBacklight(color);
}

void LcdStream::write(const char ch)
{
    lcd.write(ch);
}

}
