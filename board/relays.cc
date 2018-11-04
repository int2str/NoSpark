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

#include "board/pins.h"
#include "board/relays.h"
#include "event/loop.h"
#include "events.h"
#include "system/timer.h"

#define RELAY_TOGGLE_DELAY_MS 1000

static nospark::board::Relays *relays = 0;

ISR(TIMER2_OVF_vect) {
  if (relays) relays->updateState();
}

namespace nospark {
namespace board {

Relays::Relays()
    : state(UNKNOWN),
      enabled(false),
      sample_history(0),
      pinACRelay(PIN_AC_RELAY),
      pinDCRelay1(PIN_DC_RELAY1),
      pinDCRelay2(PIN_DC_RELAY2),
      pinSense1(PIN_AC_TEST1),
      pinSense2(PIN_AC_TEST2) {
  relays = this;

  // Start timer 2 to sample AC pins
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B |= _BV(CS22);
  TIMSK2 |= _BV(TOIE2);

  setPins(false);
}

void Relays::set(const bool enable) {
  if (enabled == enable) return;

  setPins(enable);
  enabled = enable;
  last_change = system::Timer::millis();

  event::Loop::post(event::Event(EVENT_CHARGE_STATE, enable));
}

void Relays::setPins(const bool enable) {
  pinACRelay = enable;
  pinDCRelay1 = enable;
  pinDCRelay2 = enable;
}

Relays::RelayState Relays::checkStatus() {
  // Do NOT recover from an error
  if (state != UNKNOWN && state != OK) return state;

  // Give relays time to settle
  if ((system::Timer::millis() - last_change) < RELAY_TOGGLE_DELAY_MS)
    return state;

  // Now check appropriate state...
  if (enabled)
    state = isActive() ? OK : NO_GROUND;
  else
    state = isActive() ? STUCK : OK;

  return state;
}

bool Relays::isActive() const { return (sample_history != 0); }

void Relays::updateState() {
  // This function is called about every millisecond.
  // We keep 16 bits of sample history, giving us 16ms
  // of coverage, which more than covers half a sine
  // wave at 50/60Hz.

  const bool active = !pinSense1 || !pinSense2;
  sample_history = (sample_history << 1) | active;
}

}  // namespace board
}  // namespace nospark
