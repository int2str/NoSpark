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

#define EVENT_UPDATE                1

#define EVENT_J1772_STATE          10
#define EVENT_CONTROLLER_STATE     11
#define EVENT_CHARGE_STATE         12
#define EVENT_GFCI_TRIPPED         13

#define EVENT_SET_RELAY            20

#define EVENT_KEYDOWN              30
#define EVENT_KEYHOLD              31
#define EVENT_KEYUP                32

#define EVENT_SETTINGS_CHANGED     40
#define EVENT_MAX_AMPS_CHANGED     41
#define EVENT_READY_STATE_CHANGED  42

#define EVENT_TEMPERATURE_ALERT    50

#define EVENT_REQUEST_SLEEP        60

#define EVENT_UPDATE_CHARGE_TIMER  70

#define EVENT_TEST                255
