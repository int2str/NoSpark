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
#include "evse/chargemonitor.h"
#include "evse/chargetimer.h"
#include "evse/controller.h"
#include "evse/temperaturemonitor.h"
#include "system/watchdog.h"
#include "ui/keyboard.h"
#include "ui/lcdconsole.h"
#include "ui/serialmonitor.h"

#include "events.h"

using namespace nospark;

int main()
{
    sei();

    system::Watchdog::enable();

    // Initialize event handlers
    ui::LcdConsole::init();
    ui::Keyboard::init();
    ui::SerialMonitor::init();
    evse::Controller::init();
    evse::ChargeMonitor::get();
    evse::ChargeTimer::init();
    evse::TemperatureMonitor::init();

    // Start event dispatch loop
    while (1)
    {
        event::Loop::post(event::Event(EVENT_UPDATE));
        event::Loop::dispatch();

        system::Watchdog::reset();
        board::Heartbeat::toggle();
    }
}
