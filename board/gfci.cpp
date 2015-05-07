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
#include "utils/atomic.h"
#include "events.h"
#include "gfci.h"
#include "pins.h"

#define GFCI_TEST_PULSES      50
#define GFCI_TEST_DELAY_US  8333 // ~60Hz

using event::Event;
using event::Loop;

namespace
{
    static board::GFCI *gfci = 0;

    ISR(INT0_vect)
    {
        if (gfci)
            gfci->trip();
    }

    void sendPulses(const board::GFCI &gfci, board::Pin& pin)
    {
        for (uint8_t i = 0; i != GFCI_TEST_PULSES && !gfci.isTripped(); ++i)
        {
            pin = !pin;
            _delay_us(GFCI_TEST_DELAY_US);
        }
        pin = 0;
    }
}

namespace board
{

GFCI::GFCI()
    : pinSense(PIN_GFCI_SENSE)
    , pinTest(PIN_GFCI_TEST)
    , self_test(false)
    , tripped(false)
{
    pinSense.io(Pin::IN);
    pinTest.io(Pin::OUT);

    // Pointer for ISR
    gfci = this;

    // Enable pin change interrupt
    utils::Atomic _atomic;
    EICRA = (1 << ISC00);
    EIMSK = (1 << INT0);
}

void GFCI::selfTest()
{
    tripped = false;
    self_test = true;

    sendPulses(*this, pinTest);
    Loop::post(Event(tripped ? EVENT_POST_SUCCESS : EVENT_POST_FAILED));
}

bool GFCI::isTripped() const
{
    return tripped;
}

void GFCI::trip()
{
    if (tripped)
        return;

    tripped = true;
    if (!self_test)
        Loop::post(Event(EVENT_GFCI_TRIPPED));
}

}
