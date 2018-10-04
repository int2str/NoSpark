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
#include "strings.h"

#define EOL                 "\r\n"
#define CR                  '\r'
#define LF                  '\n'

// Credit where credit is due...
char const STR_OPENEVSE[] PROGMEM =    "OpenEVSE";
char const STR_OPENEVSE_BY[] PROGMEM = "by Chris Howell";

char const STR_NOSPARK[] PROGMEM =     "NoSpark v1.00";
char const STR_NOSPARK_BY[] PROGMEM =  "by A. Eisenbach";

// Language specific
#include "strings_en.cpp"
//#include "strings_de.h"