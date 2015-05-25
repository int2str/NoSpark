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

// Console strings
#define STR_PROMPT                  PSTR("nospark> ")
#define STR_EVENT                   PSTR("\r[Event] ")
#define STR_EVT_CONTROLLER_STATE    PSTR("Controller state changed: ")
#define STR_EVT_J1772_STATE         PSTR("J1772 state changed: ")
#define STR_EVT_GFCI_TRIPPED        PSTR("GFI tripped")
#define STR_EVT_DEFAULT1            PSTR("ID: ")
#define STR_EVT_DEFAULT2            PSTR(", param: ")
#define STR_STATUS_TIME             PSTR("Time: ")
#define STR_STATUS_TEMP             PSTR("Temp: ")
#define STR_STATUS_J1772            PSTR("J1772 state: ")
#define STR_STATUS_CHARGING         PSTR("Charge state: ")
#define STR_STATUS_READY            PSTR("Ready state: ")
#define STR_STATUS_MAX_CURRENT      PSTR("Max. current: ")

// Console commands
#define STR_CMD_HELP                PSTR("help")
#define STR_CMD_RESET               PSTR("reset")
#define STR_CMD_SETCURRENT          PSTR("setcurrent")
#define STR_CMD_SETTIME             PSTR("settime")
#define STR_CMD_STATUS              PSTR("status")
#define STR_CMD_ENERGY              PSTR("energy")
#define STR_CMD_VERSION             PSTR("version")
#define STR_CMD_DEBUG               PSTR("debug")
#define STR_ERR_SETTIME_PARAM       PSTR("Invalid time format")
#define STR_ERR_PARAM               PSTR("Invalid parameter. Try 'help'.")

// Console help strings
#define STR_HELP_UNKNOWN            PSTR("Unknown command. Try 'help'.")
#define STR_HELP_COMMANDS           PSTR("Available commands:")
#define STR_HELP_HELP               PSTR("display this help")
#define STR_HELP_DEBUG              PSTR("enable (1) or disable (0) debug events")
#define STR_HELP_ENERGY             PSTR("show charge statistics")
#define STR_HELP_RESET              PSTR("force watchdog timeout to reset device")
#define STR_HELP_SETCURRENT         PSTR("set maximum charge current")
#define STR_HELP_SETTIME            PSTR("set the clock <hhmmss ddmmyy>")
#define STR_HELP_STATUS             PSTR("shows the device/charging status")
#define STR_HELP_VERSION            PSTR("displays version information")
