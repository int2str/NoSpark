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

#include "ui/serialmonitor.h"
#include "events.h"
#include "ui/strings.h"
#include "ui/strings_console.h"

#define BS 0x08
#define DEL 0x7F

using nospark::serial::Usart;

namespace nospark {
namespace ui {

SerialMonitor &SerialMonitor::init() {
  static SerialMonitor monitor;
  return monitor;
}

SerialMonitor::SerialMonitor()
    : uart(serial::Usart::get()),
      api(uart),
      console(uart),
      state(CONSOLE_STARTUP),
      echo(true),
      len(0) {}

void SerialMonitor::update() {
  char ch;

  switch (state) {
    case CONSOLE_STARTUP:
      // Good place to set BT name etc...
      state = CONSOLE_ACCUMULATING;
      break;

    case CONSOLE_ACCUMULATING:
      while (uart.avail()) {
        uart >> ch;

        if (len == 0 && ch == '$') echo = false;

        if (echo) {
          uart << ch;
          if (ch == CR) uart << LF;
        }

        if (ch == BS || ch == DEL) {
          if (len) buffer[--len] = 0;
        } else {
          if (ch != LF) buffer[len++] = ch;
        }

        if (len == CONSOLE_BUFFER || ch == CR) {
          buffer[--len] = 0;
          state = CONSOLE_COMMAND;
          break;
        }
      }
      break;

    case CONSOLE_COMMAND:
      handleCommand();
      len = 0;
      echo = true;
      state = CONSOLE_ACCUMULATING;
      break;
  }
}

void SerialMonitor::handleCommand() {
  if (len > 2 && buffer[0] == '$') {
    api.handleCommand(buffer, len);
  } else {
    console.handleCommand(buffer, len);
    uart << stream::PGM << STR_PROMPT;
  }
}

void SerialMonitor::onEvent(const event::Event &event) {
  switch (event.id) {
    case EVENT_UPDATE:
      update();
      break;

    default:
      console.onEvent(event);
      break;
  }
}

}  // namespace ui
}  // namespace nospark
