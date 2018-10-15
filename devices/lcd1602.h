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
#include "utils/pair.h"

namespace nospark {
namespace devices {

class LCD16x2 {
 public:
  enum Backlight { OFF, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE };

  enum BacklightType { RGB, MONOCHROME };

  LCD16x2();

  void clear();
  void home();
  void move(const uint8_t x, const uint8_t y);

  void createChar(const uint8_t idx, const uint8_t *data, const uint8_t len);
  void createChar_P(const uint8_t idx, const uint8_t *data, const uint8_t len);

  void setBacklight(const Backlight color);

  // |setBacklight| must be called after changing the backlight type
  void setBacklightType(const BacklightType type);

  void write(const uint8_t ch);

 private:
  devices::MCP23017 io;
  utils::Pair<uint8_t, uint8_t> backlight_bits;
  BacklightType backlight_type;

  void writeCommand(const uint8_t b);
  void pulse(const uint8_t b);
};
}
}
