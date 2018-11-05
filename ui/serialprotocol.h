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
// it online at <http://www.gnu.org/licenses/>

#pragma once

// Query with $GV
constexpr const char RAPI_VERSION[] PROGMEM = "4.0.1";

constexpr char COMMAND_TYPE_GET = 'G';
constexpr char COMMAND_TYPE_SET = 'S';
constexpr char COMMAND_TYPE_FUNCTION = 'F';

constexpr char GET_AMMETER_CALIBRATION = 'A';
constexpr char GET_MIN_MAX_AMPS = 'C';
constexpr char GET_CHARGE_TIMER = 'D';
constexpr char GET_CURRENT_AND_FLAGS = 'E';
constexpr char GET_FAULT_COUNTERS = 'F';
constexpr char GET_CURRENT = 'G';
constexpr char GET_KWH_LIMIT = 'H';
constexpr char GET_TEMPERATURE = 'P';
constexpr char GET_STATE = 'S';
constexpr char GET_TIME = 'T';
constexpr char GET_CHARGE_STATS = 'U';
constexpr char GET_VERSION = 'V';

constexpr char SET_TIME = '1';
