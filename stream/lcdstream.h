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

#include "devices/lcd1602.h"
#include "stream/outputstream.h"

namespace stream
{

class LcdStream : public OutputStream
{
public:
    LcdStream(devices::LCD16x2 &lcd);

    void clear();
    void move(const uint8_t x, const uint8_t y);

    void setBacklight(const devices::LCD16x2::Backlight color);

    devices::LCD16x2& getLCD() const;

private:
    void write(const char ch) override;
    void write(const char *string) override;

    devices::LCD16x2 &lcd;
};

}
