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

#include <avr/io.h>
#include <stdint.h>

// This us usually how one would create a "Pin"...
#define MAKEPIN(a, n, d) board::Pin(&(PIN##a), (1 << n), board::Pin::PIN_##d)

namespace nospark {
namespace board {

class Pin {
public:
  enum PinDirection { PIN_IN, PIN_IN_PULLUP, PIN_OUT };

  Pin(volatile uint8_t *reg_pin, const uint8_t b, const PinDirection d);

  uint16_t analogRead() const;

  Pin &operator=(const uint8_t rhs);
  bool operator!() const;

private:
  volatile uint8_t *const reg_pin;
  const uint8_t b;
};
}
}
