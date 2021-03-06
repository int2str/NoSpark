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

#include "strings.h"
#include "avr/pgmspace.h"

#define EOL "\r\n"
#define CR '\r'
#define LF '\n'

char const STR_NOSPARK_VER[] PROGMEM = "1.00";

// Credit where credit is due...
char const STR_OPENEVSE[] PROGMEM = "OpenEVSE";
char const STR_OPENEVSE_BY[] PROGMEM = "by Chris Howell";

char const STR_NOSPARK[] PROGMEM = "NoSpark v";
char const STR_NOSPARK_BY[] PROGMEM = "by A. Eisenbach";

// Language specific
#include "strings_en.inc"
//#include "strings_de.inc"
