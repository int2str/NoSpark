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

// General UI
#define STR_NOT_CONNECTED           PSTR("EV not connected")
#define STR_CONNECTED               PSTR("EV ready")
#define STR_CHARGING                PSTR("Charging")
#define STR_VENT_REQUIRED           PSTR("Vent required")
#define STR_ERROR_STATE             PSTR("ERROR")
#define STR_DIODE_CHECK_FAILED      PSTR("DIODE CHECK FAIL")

// Settings
#define STR_SET_CLOCK               PSTR(" Set clock")
#define STR_SET_CURRENT             PSTR(" Max. current")
#define STR_SET_RESET               PSTR(" Force restart")
#define STR_SET_EXIT                PSTR(" Exit settings")

// Console strings
#define STR_PROMPT                  PSTR("nospark> ")
#define STR_EVENT                   PSTR("\r[Event] ")
#define STR_EVT_CONTROLLER_STATE    PSTR("Controller state changed: ")
#define STR_EVT_J1772_STATE         PSTR("J1772 state changed: ")
#define STR_EVT_GFCI_TRIPPED        PSTR("GFCI tripped")
#define STR_EVT_DEFAULT1            PSTR("ID: ")
#define STR_EVT_DEFAULT2            PSTR(", param: ")
#define STR_STATUS_TIME             PSTR("Time: ")
#define STR_STATUS_TEMP             PSTR("Temp: ")

// Console commands
#define STR_CMD_HELP                PSTR("help")
#define STR_CMD_RESET               PSTR("reset")
#define STR_CMD_SETTIME             PSTR("settime ")
#define STR_CMD_STATUS              PSTR("status")
#define STR_ERR_SETTIME_PARAM       PSTR("Invalid time format")

// Console help strings
#define STR_HELP_UNKNOWN            PSTR("Unknown command. Try 'help'.")
#define STR_HELP_COMMANDS           PSTR("Available commands:")
#define STR_HELP_HELP               PSTR(" help     display this help")
#define STR_HELP_RESET              PSTR(" reset    force watchdog timeout to reset device")
#define STR_HELP_SETTIME            PSTR(" settime  set the clock <hhmmss ddmmyy>")
#define STR_HELP_STATUS             PSTR(" status   shows the device/charging status")
