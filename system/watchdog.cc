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

#include <avr/wdt.h>

#include "system/watchdog.h"

namespace nospark::system {

void Watchdog::enable() { wdt_enable(WDTO_8S); }

void Watchdog::reset() { wdt_reset(); }

void Watchdog::forceRestart() {
  wdt_enable(WDTO_15MS);
  while (1) {
  }
}

}  // namespace nospark
