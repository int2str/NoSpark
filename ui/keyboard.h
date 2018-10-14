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

#include "devices/mcp23017.h"
#include "event/handler.h"
#include "utils/cpp.h"

namespace nospark {
namespace ui {

// Currently handling a single button :)
// This is a singleton; there shall be only one.
class Keyboard : public event::Handler {
  Keyboard();
  ~Keyboard();

public:
  static Keyboard &init();

private:
  void update();
  void onEvent(const event::Event &event) override;

  // This is effectively re-defining the same IO expander twice.
  // However, this leads to better abstraction since we don't have
  // to hunt through LCD code to find our button handling.
  devices::MCP23017 io;
  uint8_t last_state;
  uint32_t last_change;

  DISALLOW_COPY_AND_ASSIGN(Keyboard);
};
}
}
