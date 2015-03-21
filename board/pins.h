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

// Buttons are defined in the following format:
//  R,B - Where "R" is the register and "B" is the bit offset
//  Example:
//    B,1 = PB1

#include "pin.h"

#define PIN_BUTTON          MAKEPIN(B,0) // Temp: Moved to I/O expander

// AC Relay pins
#define PIN_AC_RELAY        MAKEPIN(B,1)
#define PIN_AC_TEST1        MAKEPIN(D,3)
#define PIN_AC_TEST2        MAKEPIN(D,4)

// J1772 pilot uses PB2 for PWM (see j1772pilot.cpp)
#define PIN_J1772_STATUS    MAKEPIN(C,1)

// GFCI sense pin uses interrupt INT0 (=PD2; see gfci.cpp)
#define PIN_GFCI_SENSE      MAKEPIN(D,2)
#define PIN_GFCI_TEST       MAKEPIN(D,6)
