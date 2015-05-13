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

#include <stdint.h>
#include <stdbool.h>

#include "utils/cpp.h"
#include "stream/uartstream.h"

#define MAX_COMMANDS 7

namespace ui
{

class SerialConsole;
typedef void (SerialConsole::*SerialCommandHandler) (const char *buffer, const uint8_t len);

struct SerialCommand
{
    const char* command;
    const bool hasParam;
    SerialCommandHandler handler;
};

// Simple command line interface for control and monitoring.
class SerialConsole
{
public:
    SerialConsole(stream::UartStream &uart);

    void onEvent(const event::Event &event);

    bool handleCommand(const char *buffer, const uint8_t len);

private:
    void commandHelp(const char *buffer, const uint8_t len);
    void commandReset(const char *buffer, const uint8_t len);
    void commandSetCurrent(const char *buffer, const uint8_t len);
    void commandSetTime(const char *buffer, const uint8_t len);
    void commandStatus(const char *buffer, const uint8_t len);
    void commandVersion(const char *buffer, const uint8_t len);
    void commandDebug(const char *buffer, const uint8_t len);

    stream::UartStream &uart;
    bool event_debug;
    const SerialCommand commands[MAX_COMMANDS];

    DISALLOW_COPY_AND_ASSIGN(SerialConsole);
};

}
