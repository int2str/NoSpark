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

#include "event/handler.h"
#include "serial/usart.h"
#include "ui/serialapi.h"
#include "ui/serialconsole.h"
#include "utils/cpp.h"

#define CONSOLE_BUFFER 50

namespace ui
{

class SerialMonitor : public event::Handler
{
    SerialMonitor();

public:
    static SerialMonitor& init();

private:
    enum SerialState
    {
        CONSOLE_STARTUP,
        CONSOLE_ACCUMULATING,
        CONSOLE_COMMAND
    };

    void onEvent(const event::Event &event) override;

    void update();
    void handleCommand();

    serial::Usart& uart;
    ui::SerialApi api;
    ui::SerialConsole console;
    SerialState state;
    bool echo;

    char buffer[CONSOLE_BUFFER];
    uint8_t len;

    DISALLOW_COPY_AND_ASSIGN(SerialMonitor);
};

}
