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

#define EOL "\r\n"
#define CR '\r'
#define LF '\n'

extern char const STR_OPENEVSE[];
extern char const STR_OPENEVSE_BY[];
extern char const STR_NOSPARK[];
extern char const STR_NOSPARK_BY[];
extern char const STR_NOSPARK_VER[];

// General UI
extern char const STR_STATE_READY[];
extern char const STR_STATE_CONNECTED[];
extern char const STR_STATE_CHARGING[];
extern char const STR_STATE_ERROR[];
extern char const STR_NOT_CONNECTED[];
extern char const STR_CONNECTED[];
extern char const STR_CHARGING[];
extern char const STR_CHARGED[];
extern char const STR_VENT_REQUIRED[];
extern char const STR_DIODE_CHECK_FAILED[];
extern char const STR_ON[];
extern char const STR_OFF[];

// Settings
extern char const STR_SET_CLOCK[];
extern char const STR_SET_DATE[];
extern char const STR_SET_CURRENT[];
extern char const STR_SET_CHARGETIMER[];
extern char const STR_SET_KWH_LIMIT[];
extern char const STR_SET_KWH_COST[];
extern char const STR_SET_SLEEPMODE[];
extern char const STR_SET_SLEEPMODE_TIME[];
extern char const STR_SET_SLEEPMODE_OFF[];
extern char const STR_SET_SLEEPMODE_DISABLED[];
extern char const STR_SET_RESET[];
extern char const STR_SET_EXIT[];

// Faults
extern char const STR_FAULT_POST_GFCI[];
extern char const STR_FAULT_RELAY_STUCK[];
extern char const STR_FAULT_RELAY_NO_GROUND[];
extern char const STR_FAULT_GFCI_TRIPPED[];
extern char const STR_FAULT_TEMPERATURE[];

// Stats
extern char const STR_STATS_KWH[];
extern char const STR_STATS_WEEK[];
extern char const STR_STATS_MONTH[];
extern char const STR_STATS_YEAR[];
extern char const STR_STATS_TOTAL[];
