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
#define RELAY_SAMPLE_MS         13 // Capture slightly more than half an AC sine wave

#define SENSE_ACTIVE_1          0x01
#define SENSE_ACTIVE_2          0x02
#define SENSE_ACTIVE_BOTH       (SENSE_ACTIVE_1 | SENSE_ACTIVE_2)

namespace board
{

ACRelay::ACRelay()
    : state(UNKNOWN)
    , enabled(false)
    , pinACRelay(PIN_AC_RELAY)
    , pinDCRelay1(PIN_DC_RELAY1)
    , pinDCRelay2(PIN_DC_RELAY2)
    , pinSense1(PIN_AC_TEST1)
    , pinSense2(PIN_AC_TEST2)
{
}

void ACRelay::setState(const bool enable)
{
    if (enabled == enable)
        return;

    pinACRelay = enable;
    pinDCRelay1 = enable;
    pinDCRelay2 = enable;

    enabled = enable;
    last_change = system::Timer::millis();

    event::Loop::post(event::Event(EVENT_CHARGE_STATE, enable));
}

ACRelay::RelayState ACRelay::checkStatus()
{
    // Do NOT recover from an error
    if (state != UNKNOWN && state != OK)
        return state;

    // Give relays time to settle
    if ((system::Timer::millis() - last_change) < RELAY_TOGGLE_DELAY_MS)
        return state;

    // Now check appropriate state...
    if (isActive())
        state = enabled ? OK : STUCK_RELAY;
    else
        state = enabled ? GROUND_FAULT : OK;

    return state;
}

bool ACRelay::isActive() const
{
    const uint32_t start_ms = system::Timer::millis();

    while ((system::Timer::millis() - start_ms) < RELAY_SAMPLE_MS)
    {
        if (!pinSense1 || !pinSense2) // <- inputs are pulled-up; active low
            return true;
    }

    return false;
}

}
