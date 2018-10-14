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

#include "system/timer.h"
#include "utils/atomic.h"

namespace {
// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW ((64 * 256) / (F_CPU / 1000000UL))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000UL)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile uint32_t timer0_millis = 0;
uint8_t timer0_fract = 0;

ISR(TIMER0_OVF_vect) {
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)
  uint32_t m = timer0_millis + MILLIS_INC;
  uint8_t f = timer0_fract + FRACT_INC;

  if (f >= FRACT_MAX) {
    f -= FRACT_MAX;
    ++m;
  }

  timer0_fract = f;
  timer0_millis = m;
}
}

namespace nospark {
namespace system {

uint32_t Timer::millis() {
  static Timer sysTime;
  return sysTime.millis_impl();
}

Timer::Timer() {
  sei();

  // Fast PWM mode; OCRx update BOTTOM, TOV at MAX
  TCCR0A |= _BV(WGM01) | _BV(WGM00);

  // Set pre-scale factor to 64
  TCCR0B |= _BV(CS01) | _BV(CS00);

  // Enable timer 0 overflow interrupt
  TIMSK0 |= _BV(TOIE0);
}

uint32_t Timer::millis_impl() const {
  utils::Atomic _atomic;
  return timer0_millis;
}
}
}
