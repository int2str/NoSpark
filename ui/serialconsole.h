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

#include "stream/uartstream.h"
#include "utils/cpp.h"

#define MAX_COMMANDS 10

namespace nospark {
namespace ui {

class SerialConsole;
typedef void (SerialConsole::*SerialCommandHandler)(const char *param,
                                                    const uint8_t param_len);

struct SerialCommand {
  const char *command;
  SerialCommandHandler handler;
};

// Simple command line interface for control and monitoring.
class SerialConsole {
 public:
  SerialConsole(stream::UartStream &uart);

  void onEvent(const event::Event &event);

  bool handleCommand(const char *buffer, const uint8_t len);

 private:
  void commandHelp(const char *param, const uint8_t param_len);
  void commandReset(const char *param, const uint8_t param_len);
  void commandSetCurrent(const char *param, const uint8_t param_len);
  void commandSetLimit(const char *param, const uint8_t param_len);
  void commandSetTime(const char *param, const uint8_t param_len);
  void commandSetDate(const char *param, const uint8_t param_len);
  void commandStatus(const char *param, const uint8_t param_len);
  void commandVersion(const char *param, const uint8_t param_len);
  void commandDebug(const char *param, const uint8_t param_len);
  void commandEnergy(const char *param, const uint8_t param_len);

  stream::UartStream &uart;
  bool event_debug;
  const SerialCommand commands[MAX_COMMANDS];

  DISALLOW_COPY_AND_ASSIGN(SerialConsole);
};
}
}
