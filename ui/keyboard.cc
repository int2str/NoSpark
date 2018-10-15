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

#include "ui/keyboard.h"
#include "event/loop.h"
#include "events.h"
#include "system/timer.h"

#define LCD_I2C_ADDR 0x20  // LCD backpack has buttons

#define DEBOUNCE_DELAY 30
#define KEYHOLD_DELAY 600

#define BUTTON_MASK 0x01

#define KEY_DOWN 0x01
#define KEY_HOLD 0x02

namespace nospark {
namespace ui {

Keyboard &Keyboard::init() {
  static Keyboard keyboard;
  return keyboard;
}

Keyboard::Keyboard() : io(LCD_I2C_ADDR), last_state(0), last_change(0) {
  io.ioDir(BUTTON_MASK, 0x00);
  io.pullUp(BUTTON_MASK, 0x00);
}

Keyboard::~Keyboard() {}

void Keyboard::update() {
  const bool key_down = !(io.readA() & BUTTON_MASK);
  const uint32_t now = system::Timer::millis();
  const uint32_t diff = now - last_change;

  if (diff <= DEBOUNCE_DELAY) return;

  uint8_t state = last_state & KEY_HOLD;
  if (key_down) state |= KEY_DOWN;

  if (key_down && (last_state & KEY_DOWN) != 0 && diff > KEYHOLD_DELAY)
    state |= KEY_HOLD;

  if (state == last_state) return;

  if (state & KEY_DOWN && state & KEY_HOLD)
    event::Loop::post(event::Event(EVENT_KEYHOLD));

  else if (state & KEY_DOWN)
    event::Loop::post(event::Event(EVENT_KEYDOWN));

  else if (state == 0 && (last_state & KEY_HOLD) == 0)
    event::Loop::post(event::Event(EVENT_KEYUP));

  last_change = now;
  last_state = key_down ? state : 0;
}

void Keyboard::onEvent(const event::Event &event) {
  if (event.id == EVENT_UPDATE) update();
}
}
}
