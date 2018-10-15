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

#include "devices/lcd1602.h"
#include "event/handler.h"
#include "lcdstate.h"
#include "stream/lcdstream.h"
#include "utils/cpp.h"

namespace nospark {
namespace ui {

// LCD UI.
// This is a singleton; there shall be only one.
class LcdConsole : public event::Handler {
  LcdConsole();

 public:
  static LcdConsole &init();

 protected:
  void onEvent(const event::Event &event) override;

 private:
  void update();
  void updateSleepState(const event::Event &event);
  void setState(LcdState *newState);

  bool in_settings;
  bool sleeping;
  uint32_t last_event;

  LcdState *lcdState;
  stream::LcdStream lcd;
  devices::LCD16x2 lcd_int;

  DISALLOW_COPY_AND_ASSIGN(LcdConsole);
};
}
}
