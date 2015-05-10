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

// 16-char LCD ruler --->                |01234567890123456|

// General UI                            |01234567890123456|
#define STR_NOT_CONNECTED           PSTR("Nicht verbunden")
#define STR_CONNECTED               PSTR("EV bereit")
#define STR_CHARGING                PSTR("Laedt auf ")
#define STR_CHARGED                 PSTR("Geladen")
#define STR_VENT_REQUIRED           PSTR("Abzug notwendig")
#define STR_ERROR_STATE             PSTR("FEHLER")
#define STR_DIODE_CHECK_FAILED      PSTR("DIODE TEST FEHLER")
#define STR_OFF                     PSTR("aus")

// Settings                              |01234567890123456|
#define STR_SET_CLOCK               PSTR(" Uhrzeit")
#define STR_SET_DATE                PSTR(" Datum")
#define STR_SET_CURRENT             PSTR(" Max. Ampere")
#define STR_SET_KWH_LIMIT           PSTR(" kWh Limit")
#define STR_SET_SLEEPMODE           PSTR(" Schlaf modus")
#define STR_SET_SLEEPMODE_TIME      PSTR("Zeit anzeigen")
#define STR_SET_SLEEPMODE_OFF       PSTR("Display aus")
#define STR_SET_SLEEPMODE_DISABLED  PSTR("Immer an")
#define STR_SET_RESET               PSTR(" Neustart")
#define STR_SET_EXIT                PSTR(" Zurueck")

// Faults                                |01234567890123456|
#define STR_FAULT_POST_GFCI         PSTR("GFI Test")
#define STR_FAULT_POST_RELAY        PSTR("Relay Test")
#define STR_FAULT_GFCI_TRIPPED      STR_EVT_GFCI_TRIPPED
#define STR_FAULT_TEMPERATURE       PSTR("Temperatur Alarm")

// Stats                                 |01234567890123456|
#define STR_STATS_KWH               PSTR("kWh aufgeladen")
#define STR_STATS_WEEK              PSTR("Woche")
#define STR_STATS_MONTH             PSTR("Monat")
#define STR_STATS_YEAR              PSTR("Jahr")
#define STR_STATS_TOTAL             PSTR("Gesamt")

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
#define STR_CMD_SETCURRENT          PSTR("setcurrent ")
#define STR_CMD_SETTIME             PSTR("settime ")
#define STR_CMD_STATUS              PSTR("status")
#define STR_CMD_DEBUG               PSTR("debug ")
#define STR_ERR_SETTIME_PARAM       PSTR("Invalid time format")
#define STR_ERR_PARAM               PSTR("Invalid parameter. Try 'help'.")

// Console help strings
#define STR_HELP_UNKNOWN            PSTR("Unknown command. Try 'help'.")
#define STR_HELP_COMMANDS           PSTR("Available commands:")
#define STR_HELP_HELP               PSTR(" help         display this help")
#define STR_HELP_DEBUG              PSTR(" debug        enable (1) or disable (0) debug events")
#define STR_HELP_RESET              PSTR(" reset        force watchdog timeout to reset device")
#define STR_HELP_SETCURRENT         PSTR(" setcurrent   set maximum charge current")
#define STR_HELP_SETTIME            PSTR(" settime      set the clock <hhmmss ddmmyy>")
#define STR_HELP_STATUS             PSTR(" status       shows the device/charging status")
