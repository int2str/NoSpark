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

#include "board/ammeter.h"
#include "board/adc.h"

// Apparently this is calculated from the burden
// resistor (22 Ohm in the OpenEVSE v3) and the
// number of ADC steps.
//     5V / 1024 steps = 4.882mV/step
//     22 Ohm / 4.882mV = 4.5056
//     1 / 4.5056 * 1000 (to get to mA) = 222
//
// Slightly tweaked for accuracy and more aggressive
// ADC sampling....
#define CURRENT_SCALE_FACTOR 200l

namespace nospark {
namespace board {

Ammeter::Ammeter() : last_sample(0) {}

uint32_t Ammeter::sample() {
  if (Adc::get().ammeterReady())
    last_sample = Adc::get().readAmmeter() * CURRENT_SCALE_FACTOR;

  return last_sample;
}
}
}
