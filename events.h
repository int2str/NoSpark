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

// !!! EVENT NUMBERS MUST BE UNIQUE !!!
// !!! Must also fit inside uint8_t !!!

#define EVENT_UPDATE              0

#define EVENT_J1772_STATE        10
#define EVENT_CONTROLLER_STATE   11

#define EVENT_GFCI_TRIPPED       20

#define EVENT_KEYDOWN            30
#define EVENT_KEYHOLD            31
#define EVENT_KEYUP              32

#define EVENT_POST_BEGIN        100
#define EVENT_POST_SUCCESS      101
#define EVENT_POST_FAILED       102
#define EVENT_POST_COMPLETED    103
#define EVENT_POST_GFCI         110
#define EVENT_POST_ACRELAY      111

#define EVENT_TEST              255
