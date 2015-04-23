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

#include <avr/interrupt.h>

#include "board/heartbeat.h"
#include "event/loop.h"
#include "evse/controller.h"
#include "evse/chargemonitor.h"
#include "evse/temperaturemonitor.h"
#include "system/watchdog.h"
#include "ui/keyboard.h"
#include "ui/lcdconsole.h"
#include "ui/serialconsole.h"

#include "events.h"

int main()
{
    sei();

    system::Watchdog::enable();

    ui::LcdConsole::init();
    ui::Keyboard::init();
    ui::SerialConsole::init();
    evse::Controller::init();
    evse::ChargeMonitor::get();
    evse::TemperatureMonitor::init();

    while (1)
    {
        event::Loop::post(event::Event());
        event::Loop::dispatch();

        system::Watchdog::reset();
        board::Heartbeat::toggle();
    }
}
