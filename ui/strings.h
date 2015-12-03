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

#include "avr/pgmspace.h"

#define EOL                 "\r\n"
#define CR                  '\r'
#define LF                  '\n'

// Credit where credit is due...
#define STR_OPENEVSE        PSTR("OpenEVSE")
#define STR_OPENEVSE_BY     PSTR("by Chris Howell")

#define STR_NOSPARK         PSTR("NoSpark v1.00")
#define STR_NOSPARK_BY      PSTR("by A. Eisenbach")

#define STR_EXTRA           PSTR("Hacked 20151202")
#define STR_EXTRA_BY        PSTR("by W. McBrine")

// Language specific
#include "strings_en.h"
//#include "strings_de.h"
