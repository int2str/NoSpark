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
#include "board/acrelay.h"
#include "board/pins.h"
#include "system/timer.h"
#include "events.h"

#define RELAY_TOGGLE_DELAY_MS   150
#define RELAY_SAMPLE_MS         20 // Capture full AC sine wave

#define SENSE_ACTIVE_1          0x01
#define SENSE_ACTIVE_2          0x02
#define SENSE_ACTIVE_BOTH       (SENSE_ACTIVE_1 | SENSE_ACTIVE_2)

using event::Event;
using event::Loop;

namespace board
{

ACRelay::ACRelay()
    : state(UNKNOWN)
    , pinACRelay(PIN_AC_RELAY)
    , pinDCRelay1(PIN_DC_RELAY1)
    , pinDCRelay2(PIN_DC_RELAY2)
    , pinSense1(PIN_AC_TEST1)
    , pinSense2(PIN_AC_TEST2)
{
    pinACRelay.io(Pin::OUT);
    pinDCRelay1.io(Pin::OUT);
    pinDCRelay2.io(Pin::OUT);
    pinSense1.io(Pin::IN_PULLUP);
    pinSense2.io(Pin::IN_PULLUP);

    disable();
}

void ACRelay::enable()
{
    pinACRelay = 1;
    pinDCRelay1 = 1;
    pinDCRelay2 = 1;
}

void ACRelay::disable()
{
    pinACRelay = 0;
    pinDCRelay1 = 0;
    pinDCRelay2 = 0;
}

void ACRelay::selfTest(const bool evPresent)
{
    disable();
    _delay_ms(RELAY_TOGGLE_DELAY_MS);
    uint8_t active_idle = getActive();

    // Make sure off means off...
    if (active_idle)
    {
        state = ERROR;
        Loop::post(Event(EVENT_POST_FAILED));
        return;
    }

    // If the car is connected, we can't play with the relays :(
    if (evPresent)
    {
        state = UNKNOWN;
        Loop::post(Event(EVENT_POST_SUCCESS));
        return;
    }

    // Check AC relay first
    const uint8_t active_ac = testRelay(pinACRelay);
    active_idle = getActive();

    // If the AC relay did something, our work here is done...
    if (active_ac != 0)
    {
        state = active_ac == SENSE_ACTIVE_BOTH && active_idle == 0 ? L2_READY : ERROR;
        Loop::post(Event(state != ERROR ? EVENT_POST_SUCCESS : EVENT_POST_FAILED));
        return;
    }

    // Check DC relays one at a time
    const uint8_t active_dc1 = testRelay(pinDCRelay1);
    const uint8_t active_dc2 = testRelay(pinDCRelay2);
    active_idle = getActive();

    if (active_idle)
    {
        state = ERROR; // Relay should not be active after test
    } else {
        const RelayState STATE_MAP[4][4] = {
          //     OFF   | ACTIVE_1 | ACTIVE_2 | ACTIVE_BOTH
            { ERROR    , L1_READY , L1_READY , L2_READY    } // OFF
          , { L1_READY , ERROR    , L2_READY , ERROR       } // ACTIVE_1
          , { L1_READY , L2_READY , ERROR    , ERROR       } // ACTIVE_2
          , { L2_READY , ERROR    , ERROR    , ERROR       } // ACTIVE_BOTH
        };

        state = STATE_MAP[active_dc1][active_dc2];
    }

    Loop::post(Event(state != ERROR ? EVENT_POST_SUCCESS : EVENT_POST_FAILED));
}

uint8_t ACRelay::getActive() const
{
    const uint32_t start_ms = system::Timer::millis();
    uint8_t active = 0;

    while (active != SENSE_ACTIVE_BOTH && (system::Timer::millis() - start_ms) < RELAY_SAMPLE_MS)
    {
        if (!pinSense1) // <- input is puleld-up; active low
            active |= SENSE_ACTIVE_1;
        if (!pinSense2) // <- input is puleld-up; active low
            active |= SENSE_ACTIVE_2;
    }

    return active;
}

uint8_t ACRelay::testRelay(board::Pin &pin)
{
    pin = 1;
    _delay_ms(RELAY_TOGGLE_DELAY_MS);

    uint8_t active = getActive();

    disable();
    _delay_ms(RELAY_TOGGLE_DELAY_MS);

    return active;
}

}
