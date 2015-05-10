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

// 16-char LCD ruler --->                |1234567890123456|

// General UI                            |1234567890123456|
#define STR_NOT_CONNECTED           PSTR("EV not connected")
#define STR_CONNECTED               PSTR("EV ready")
#define STR_CHARGING                PSTR("Charging  ")
#define STR_CHARGED                 PSTR("Charged")
#define STR_VENT_REQUIRED           PSTR("Vent required")
#define STR_ERROR_STATE             PSTR("ERROR")
#define STR_DIODE_CHECK_FAILED      PSTR("DIODE CHECK FAIL")
#define STR_ON                      PSTR("on")
#define STR_OFF                     PSTR("off")

// Settings                              |1234567890123456|
#define STR_SET_CLOCK               PSTR(" Set time")
#define STR_SET_DATE                PSTR(" Set date")
#define STR_SET_CURRENT             PSTR(" Max. current")
#define STR_SET_CHARGETIMER         PSTR(" Charge timer")
#define STR_SET_KWH_LIMIT           PSTR(" Charge limit")
#define STR_SET_SLEEPMODE           PSTR(" Sleep mode")
#define STR_SET_SLEEPMODE_TIME      PSTR("show time")
#define STR_SET_SLEEPMODE_OFF       PSTR("display off")
#define STR_SET_SLEEPMODE_DISABLED  PSTR("always on")
#define STR_SET_RESET               PSTR(" Force restart")
#define STR_SET_EXIT                PSTR(" Exit settings")

// Faults                                |1234567890123456|
#define STR_FAULT_POST_GFCI         PSTR("GFI test failed")
#define STR_FAULT_POST_RELAY        PSTR("Relay test fail")
#define STR_FAULT_GFCI_TRIPPED      PSTR("GFI tripped")
#define STR_FAULT_TEMPERATURE       PSTR("High temperature")

// Stats                                 |1234567890123456|
#define STR_STATS_KWH               PSTR("Total kW-hours")
#define STR_STATS_WEEK              PSTR("This week")
#define STR_STATS_MONTH             PSTR("This month")
#define STR_STATS_YEAR              PSTR("This year")
#define STR_STATS_TOTAL             PSTR("Lifetime")

