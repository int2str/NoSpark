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
#define STR_NOT_CONNECTED           PSTR("Nicht verbunden")
#define STR_CONNECTED               PSTR("EV bereit")
#define STR_CHARGING                PSTR("Laedt auf ")
#define STR_CHARGED                 PSTR("Geladen")
#define STR_VENT_REQUIRED           PSTR("Abzug notwendig")
#define STR_ERROR_STATE             PSTR("FEHLER")
#define STR_DIODE_CHECK_FAILED      PSTR("DIODEN FEHLER")
#define STR_OFF                     PSTR("aus")

// Settings                              |1234567890123456|
#define STR_SET_CLOCK               PSTR(" Uhrzeit")
#define STR_SET_DATE                PSTR(" Datum")
#define STR_SET_CURRENT             PSTR(" Max. Ampere")
#define STR_SET_CHARGETIMER         PSTR(" Lade Zeiten")
#define STR_SET_KWH_LIMIT           PSTR(" kWh Limit")
#define STR_SET_SLEEPMODE           PSTR(" Schlaf modus")
#define STR_SET_SLEEPMODE_TIME      PSTR("Zeit anzeigen")
#define STR_SET_SLEEPMODE_OFF       PSTR("Display aus")
#define STR_SET_SLEEPMODE_DISABLED  PSTR("Immer an")
#define STR_SET_RESET               PSTR(" Neustart")
#define STR_SET_EXIT                PSTR(" Zurueck")

// Faults                                |1234567890123456|
#define STR_FAULT_POST_GFCI         PSTR("GFI Test")
#define STR_FAULT_POST_RELAY        PSTR("Relay Test")
#define STR_FAULT_GFCI_TRIPPED      PSTR("GFI Fehler")
#define STR_FAULT_TEMPERATURE       PSTR("Temperatur Alarm")

// Stats                                 |1234567890123456|
#define STR_STATS_KWH               PSTR("kWh aufgeladen")
#define STR_STATS_WEEK              PSTR("Woche")
#define STR_STATS_MONTH             PSTR("Monat")
#define STR_STATS_YEAR              PSTR("Jahr")
#define STR_STATS_TOTAL             PSTR("Gesamt")

