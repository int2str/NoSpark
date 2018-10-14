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

// clang-format off

// General UI                            |1234567890123456|
#define STR_STATE_READY             PSTR("BEREIT   ")
#define STR_STATE_CONNECTED         PSTR("VERBUNDEN")
#define STR_STATE_CHARGING          PSTR("AUFLADEN ")
#define STR_STATE_ERROR             PSTR("FEHLER   ")
#define STR_NOT_CONNECTED           PSTR("Nicht verbunden")
#define STR_CONNECTED               PSTR("EV bereit")
#define STR_CHARGING                PSTR("Aufladen")
#define STR_CHARGED                 PSTR("Geladen")
#define STR_VENT_REQUIRED           PSTR("Abzug notwendig")
#define STR_DIODE_CHECK_FAILED      PSTR("DIODEN FEHLER")
#define STR_ON                      PSTR("an")
#define STR_OFF                     PSTR("aus")

// Settings                              |1234567890123456|
#define STR_SET_CLOCK               PSTR(" Uhrzeit")
#define STR_SET_DATE                PSTR(" Datum")
#define STR_SET_CURRENT             PSTR(" Max. Ampere")
#define STR_SET_CHARGETIMER         PSTR(" Lade Zeiten")
#define STR_SET_KWH_LIMIT           PSTR(" kWh Limit")
#define STR_SET_KWH_COST            PSTR(" Preis pro kWh")
#define STR_SET_SLEEPMODE           PSTR(" Schlaf modus")
#define STR_SET_SLEEPMODE_TIME      PSTR("Zeit anzeigen")
#define STR_SET_SLEEPMODE_OFF       PSTR("Display aus")
#define STR_SET_SLEEPMODE_DISABLED  PSTR("Immer an")
#define STR_SET_LCD_TYPE            PSTR(" Beleuchtung")
#define STR_SET_TYPE_RGB            PSTR("Farbe (RGB)")
#define STR_SET_TYPE_MONO           PSTR("Monochrom")
#define STR_SET_RESET               PSTR(" Neustart")
#define STR_SET_EXIT                PSTR(" Zurueck")

// Faults                                |1234567890123456|
#define STR_FAULT_POST_GFCI         PSTR("GFI Selbsttest")
#define STR_FAULT_RELAY_STUCK       PSTR("Relay haengt")
#define STR_FAULT_RELAY_NO_GROUND   PSTR("Erdungs Fehler")
#define STR_FAULT_GFCI_TRIPPED      PSTR("GFI Fehler")
#define STR_FAULT_TEMPERATURE       PSTR("Temperatur Alarm")

// Stats                                 |1234567890123456|
#define STR_STATS_KWH               PSTR("Lade Statistik:")
#define STR_STATS_WEEK              PSTR("Diese Woche")
#define STR_STATS_MONTH             PSTR("Dieser Monat")
#define STR_STATS_YEAR              PSTR("Dieses Jahr")
#define STR_STATS_TOTAL             PSTR("Gesamt")

// clang-format on
