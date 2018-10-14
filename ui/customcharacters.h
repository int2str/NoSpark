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
#include <avr/pgmspace.h>

namespace nospark {
namespace ui {

class CustomCharacters {
public:
  enum CUSTOM_CHARS_MAP {
    SEPARATOR,
    CLOCK,
    CALENDAR,
    BOLT,
    HOURGLASS,
    ZZ,
    BATTERY1,
    EURO
  };

  static void loadCustomChars(devices::LCD16x2 &lcd);
  static void loadLargeDigits(devices::LCD16x2 &lcd);
  static void largeDigit(devices::LCD16x2 &lcd, const uint8_t digit,
                         const uint8_t offset);
};
}
}
