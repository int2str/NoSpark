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

#include "board/gfci.h"
#include "board/pins.h"
#include "event/loop.h"
#include "events.h"
#include "utils/atomic.h"

#define GFCI_TEST_PULSES 100
#define GFCI_TEST_DELAY_US 8333 // ~60Hz
#define GFCI_RESET_DELAY_MS 500

static nospark::board::GFCI *gfci = 0;

ISR(INT0_vect) {
  if (gfci)
    gfci->trip();
}

namespace nospark {
namespace board {

GFCI::GFCI()
    : pinSense(PIN_GFCI_SENSE), pinTest(PIN_GFCI_TEST), self_test(false),
      tripped(false) {
  // Pointer for ISR
  gfci = this;

  // Enable pin change interrupt (rising edge)
  utils::Atomic _atomic;
  EICRA = (1 << ISC01) || (1 << ISC00);
  EIMSK = (1 << INT0);
}

void GFCI::sendPulses() {
  for (uint8_t i = 0; i != GFCI_TEST_PULSES && !tripped; ++i) {
    pinTest = !pinTest;
    _delay_us(GFCI_TEST_DELAY_US);
  }
  pinTest = 0;
}

bool GFCI::selfTest() {
  if (tripped)
    return false;

  bool result = false;
  self_test = true;

  // Generate pulses to trip GFI
  sendPulses();

  // Wait for pin to go low again
  uint8_t retries = 0;
  while (++retries && !!pinSense)
    _delay_ms(10);

  // Extra delay to let peak hold cap bleed off
  _delay_ms(GFCI_RESET_DELAY_MS);

  // Check if we're back low...
  result = tripped && !pinSense;

  self_test = false;
  tripped = false;

  return result;
}

void GFCI::trip() {
  if (tripped)
    return;

  tripped = true;

  if (!self_test)
    event::Loop::post(event::Event(EVENT_GFCI_TRIPPED));
}
}
}
