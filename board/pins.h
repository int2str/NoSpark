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

// clang-format off

// AC Relay pins
#define PIN_AC_RELAY        MAKEPIN(B, 1, OUT)
#define PIN_DC_RELAY1       MAKEPIN(B, 0, OUT)
#define PIN_DC_RELAY2       MAKEPIN(D, 7, OUT)
#define PIN_AC_TEST1        MAKEPIN(D, 3, IN_PULLUP)
#define PIN_AC_TEST2        MAKEPIN(D, 4, IN_PULLUP)

// Current probe
#define PIN_AMMETER         MAKEPIN(C, 0, IN)

// J1772 pilot uses PB2 for PWM (see j1772pilot.cpp)
#define PIN_J1772_STATUS    MAKEPIN(C, 1, IN)

// GFCI sense pin uses interrupt INT0 (=PD2; see gfci.cpp)
#define PIN_GFCI_SENSE      MAKEPIN(D, 2, IN)
#define PIN_GFCI_TEST       MAKEPIN(D, 6, OUT)

// Debug/testing pins
#define PIN_SCK             MAKEPIN(B, 5, OUT) // Heartbeat
#define PIN_MISO            MAKEPIN(B, 4, OUT) // Sleep
#define PIN_MOSI            MAKEPIN(B, 3, OUT) // Available (PWM)

// clang-format on
