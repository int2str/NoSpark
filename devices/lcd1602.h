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

#include "devices/mcp23017.h"

namespace devices
{

class LCD16x2
{
public:
    enum Backlight
    {
        OFF,
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CYAN,
        MAGENTA,
        WHITE
    };

    LCD16x2();

    void clear();
    void home();
    void move(const uint8_t x, const uint8_t y);

    void createChar(const uint8_t idx, const uint8_t* data, const uint8_t len);

    void setBacklight(const Backlight color);

    void write(const char* str);
    void write_P(const char* str);
    void write(const uint8_t ch);

private:
    devices::MCP23017 io;
    Backlight backlight;

    void writeCommand(const uint8_t b);
    void writeData(const uint8_t b);
    void pulse(const uint8_t b);
};

}
