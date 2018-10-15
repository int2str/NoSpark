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

#ifdef __GNUC__
#define _INLINE_ __attribute__((always_inline)) inline
#else
#define _INLINE_ inline
#endif

#include <avr/interrupt.h>

namespace nospark {
namespace utils {

class Atomic {
 public:
  _INLINE_ Atomic() { cli(); }

  _INLINE_ ~Atomic() { sei(); }
};
}
}
