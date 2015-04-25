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
#include <util/delay.h>

#include "event/loop.h"
#include "acrelay.h"
#include "events.h"
#include "pins.h"

#define RELAY_TOGGLE_DELAY_MS   150

using event::Event;
using event::Loop;

namespace board
{

ACRelay& ACRelay::get()
{
    static ACRelay relay;
    return relay;
}

ACRelay::ACRelay()
    : pinRelay(PIN_AC_RELAY)
    , pinSense(PIN_AC_TEST1)
    , pinSenseLegacy(PIN_AC_TEST2)
{
    pinRelay.io(Pin::OUT);
    pinSense.io(Pin::IN_PULLUP);
    pinSenseLegacy.io(Pin::IN_PULLUP);

    disable();
}

void ACRelay::enable()
{
    pinRelay = 1;
}

void ACRelay::disable()
{
    pinRelay = 0;
}

bool ACRelay::isActive() const
{
    return !pinSense;
}

void ACRelay::selfTest(const bool evPresent)
{
    disable();
    _delay_ms(RELAY_TOGGLE_DELAY_MS);

    if (isActive())
    {
        Loop::post(Event(EVENT_POST_FAILED));
        return;
    }

    if (!evPresent)
    {
        enable();
        _delay_ms(RELAY_TOGGLE_DELAY_MS);

        const bool active = isActive();
        _delay_ms(RELAY_TOGGLE_DELAY_MS);

        disable();

        Loop::post(Event(active ? EVENT_POST_SUCCESS : EVENT_POST_FAILED));
    } else {
        Loop::post(Event(EVENT_POST_SUCCESS));
    }
}

}
