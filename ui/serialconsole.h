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

#include "event/handler.h"
#include "serial/usart.h"
#include "utils/cpp.h"

#define CONSOLE_BUFFER 50

#define MAX_COMMANDS    4

namespace ui
{

class SerialConsole;
typedef void (SerialConsole::*SerialCommandHandler) ();

struct SerialCommand
{
    const char* command;
    const bool hasParam;
    SerialCommandHandler handler;
};

// Simple command line interface for control and monitoring.
// This is a singleton; there shall be only one.
class SerialConsole : public event::Handler
{
    SerialConsole();

public:
    static SerialConsole& init();

private:
    enum State
    {
        CONSOLE_STARTUP,
        CONSOLE_READY,
        CONSOLE_ACCUMULATING,
        CONSOLE_COMMAND
    };

    void onEvent(const event::Event &event);

    State handleCommand();
    void commandHelp();
    void commandReset();
    void commandSetTime();
    void commandStatus();
    void update();

    serial::Usart& uart;
    State state;

    char buffer[CONSOLE_BUFFER];
    uint8_t len;

    const SerialCommand commands[MAX_COMMANDS];

    DISALLOW_COPY_AND_ASSIGN(SerialConsole);
};

}
