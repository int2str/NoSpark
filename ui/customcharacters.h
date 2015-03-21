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

#include <avr/pgmspace.h>

#define NUM_CUSTOM_CHARS 8

namespace ui
{

const PROGMEM uint8_t CUSTOM_CHAR_MAP[NUM_CUSTOM_CHARS][8] = {
    {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04} // CUSTOM_CHAR_SEPARATOR
  , {0x00, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00, 0x00} // CUSTOM_CHAR_CLOCK
  , {0x0e, 0x1f, 0x11, 0x11, 0x11, 0x11, 0x1f, 0x00} // CUSTOM_CHAR_BATTERY0
  , {0x0e, 0x1f, 0x11, 0x11, 0x11, 0x1f, 0x1f, 0x00} // CUSTOM_CHAR_BATTERY1
  , {0x0e, 0x1f, 0x11, 0x11, 0x1f, 0x1f, 0x1f, 0x00} // CUSTOM_CHAR_BATTERY2
  , {0x0e, 0x1f, 0x11, 0x1f, 0x1f, 0x1f, 0x1f, 0x00} // CUSTOM_CHAR_BATTERY3
  , {0x0e, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00} // CUSTOM_CHAR_BATTERY4
  , {0x07, 0x0e, 0x0c, 0x1f, 0x03, 0x06, 0x0c, 0x08} // CUSTOM_CHAR_BOLT
};

enum CUSTOM_CHARS
{
    CUSTOM_CHAR_SEPARATOR
  , CUSTOM_CHAR_CLOCK
  , CUSTOM_CHAR_BATTERY0
  , CUSTOM_CHAR_BATTERY1
  , CUSTOM_CHAR_BATTERY2
  , CUSTOM_CHAR_BATTERY3
  , CUSTOM_CHAR_BATTERY4
  , CUSTOM_CHAR_BOLT
};

}