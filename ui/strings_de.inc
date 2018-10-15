// NoSpark - OpenEVSE charger firmware
// Copyright (C 2015 Andre Eisenbach
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

// General UI                                 |1234567890123456|
const char STR_STATE_READY[] PROGMEM =        "BEREIT   ";
const char STR_STATE_CONNECTED[] PROGMEM =    "VERBUNDEN";
const char STR_STATE_CHARGING[] PROGMEM =     "AUFLADEN ";
const char STR_STATE_ERROR[] PROGMEM =        "FEHLER   ";
const char STR_NOT_CONNECTED[] PROGMEM =      "Nicht verbunden";
const char STR_CONNECTED[] PROGMEM =          "EV bereit";
const char STR_CHARGING[] PROGMEM =           "Aufladen";
const char STR_CHARGED[] PROGMEM =            "Geladen";
const char STR_VENT_REQUIRED[] PROGMEM =      "Abzug notwendig";
const char STR_DIODE_CHECK_FAILED[] PROGMEM = "DIODEN FEHLER";
const char STR_ON[] PROGMEM =                 "an";
const char STR_OFF[] PROGMEM =                "aus";

// Settings                                   |1234567890123456|
const char STR_SET_CLOCK[] PROGMEM =          " Uhrzeit";
const char STR_SET_DATE[] PROGMEM =           " Datum";
const char STR_SET_CURRENT[] PROGMEM =        " Max. Ampere";
const char STR_SET_CHARGETIMER[] PROGMEM =    " Lade Zeiten";
const char STR_SET_KWH_LIMIT[] PROGMEM =      " kWh Limit";
const char STR_SET_KWH_COST[] PROGMEM =       " Preis pro kWh";
const char STR_SET_SLEEPMODE[] PROGMEM =      " Schlaf modus";
const char STR_SET_SLEEPMODE_TIME[] PROGMEM = "Zeit anzeigen";
const char STR_SET_SLEEPMODE_OFF[] PROGMEM =  "Display aus";
const char STR_SET_SLEEPMODE_DISABLED[] PROGMEM = "Immer an";
const char STR_SET_RESET[] PROGMEM =          " Neustart";
const char STR_SET_EXIT[] PROGMEM =           " Zurueck";

// Faults                                     |1234567890123456|
const char STR_FAULT_POST_GFCI[] PROGMEM    = "GFI Selbsttest";
const char STR_FAULT_RELAY_STUCK[] PROGMEM =  "Relay haengt";
const char STR_FAULT_RELAY_NO_GROUND[] PROGMEM = "Erdungs Fehler";
const char STR_FAULT_GFCI_TRIPPED[] PROGMEM = "GFI Fehler";
const char STR_FAULT_TEMPERATURE[] PROGMEM =  "Temperatur Alarm";

// Stats                                      |1234567890123456|
const char STR_STATS_KWH[] PROGMEM =          "Lade Statistik:";
const char STR_STATS_WEEK[] PROGMEM =         "Diese Woche";
const char STR_STATS_MONTH[] PROGMEM =        "Dieser Monat";
const char STR_STATS_YEAR[] PROGMEM =         "Dieses Jahr";
const char STR_STATS_TOTAL[] PROGMEM =        "Gesamt";

// clang-format on
