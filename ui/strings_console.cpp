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

#include "avr/pgmspace.h"
#include "strings_console.h"

// Console strings
char const STR_PROMPT[] PROGMEM = "nospark> ";
char const STR_EVENT[] PROGMEM = "\r[Event] ";
char const STR_EVT_CONTROLLER_STATE[] PROGMEM = "Controller state changed: ";
char const STR_EVT_J1772_STATE[] PROGMEM = "J1772 state changed: ";
char const STR_EVT_GFCI_TRIPPED[] PROGMEM = "GFI tripped";
char const STR_EVT_DEFAULT1[] PROGMEM = "ID: ";
char const STR_EVT_DEFAULT2[] PROGMEM = ", param: ";
char const STR_STATUS_TIME[] PROGMEM = "Time: ";
char const STR_STATUS_TEMP[] PROGMEM = "Temp: ";
char const STR_STATUS_J1772[] PROGMEM = "J1772 state: ";
char const STR_STATUS_CHARGING[] PROGMEM = "Charge state: ";
char const STR_STATUS_READY[] PROGMEM = "Ready state: ";
char const STR_STATUS_MAX_CURRENT[] PROGMEM = "Max. current: ";
char const STR_STATUS_LIMIT[] PROGMEM = "Charge limit: ";

// Console commands
char const STR_CMD_HELP[] PROGMEM = "help";
char const STR_CMD_RESET[] PROGMEM = "reset";
char const STR_CMD_SETCURRENT[] PROGMEM = "setcurrent";
char const STR_CMD_SETLIMIT[] PROGMEM = "setlimit";
char const STR_CMD_SETTIME[] PROGMEM = "settime";
char const STR_CMD_STATUS[] PROGMEM = "status";
char const STR_CMD_ENERGY[] PROGMEM = "energy";
char const STR_CMD_VERSION[] PROGMEM = "version";
char const STR_CMD_DEBUG[] PROGMEM = "debug";
char const STR_ERR_SETTIME_PARAM[] PROGMEM = "Invalid time format";
char const STR_ERR_PARAM[] PROGMEM = "Invalid parameter. Try 'help'.";

// Console help strings
char const STR_HELP_UNKNOWN[] PROGMEM = "Unknown command. Try 'help'.";
char const STR_HELP_COMMANDS[] PROGMEM = "Available commands:";
char const STR_HELP_HELP[] PROGMEM = "display this help";
char const STR_HELP_DEBUG[] PROGMEM = "enable (1) or disable (0) debug events";
char const STR_HELP_ENERGY[] PROGMEM = "show charge statistics";
char const STR_HELP_RESET[] PROGMEM = "force watchdog timeout to reset device";
char const STR_HELP_SETCURRENT[] PROGMEM = "set maximum charge current (A)";
char const STR_HELP_SETLIMIT[] PROGMEM = "set maximum kWh per charge";
char const STR_HELP_SETTIME[] PROGMEM = "set the clock <hhmmss ddmmyy>";
char const STR_HELP_STATUS[] PROGMEM = "shows the device/charging status";
char const STR_HELP_VERSION[] PROGMEM = "displays version information";
