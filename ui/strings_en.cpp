// NoSpark - OpenEVSE charger firmware
// Copyright C 2015 Andre Eisenbach
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

// General UI                                     |1234567890123456|
char const STR_STATE_READY[] PROGMEM =            "READY    ";
char const STR_STATE_CONNECTED[] PROGMEM =        "CONNECTED";
char const STR_STATE_CHARGING[] PROGMEM =         "CHARGING ";
char const STR_STATE_ERROR[] PROGMEM =            "ERROR    ";
char const STR_NOT_CONNECTED[] PROGMEM =          "EV not connected";
char const STR_CONNECTED[] PROGMEM =              "EV ready";
char const STR_CHARGING[] PROGMEM =               "Charging";
char const STR_CHARGED[] PROGMEM =                "Charged";
char const STR_VENT_REQUIRED[] PROGMEM =          "Vent required";
char const STR_DIODE_CHECK_FAILED[] PROGMEM =     "DIODE CHECK FAIL";
char const STR_ON[] PROGMEM =                     "on";
char const STR_OFF[] PROGMEM =                    "off";

// Settings                                       |1234567890123456|
char const STR_SET_CLOCK[] PROGMEM =              " Set time";
char const STR_SET_DATE[] PROGMEM =               " Set date";
char const STR_SET_CURRENT[] PROGMEM =            " Max. current";
char const STR_SET_CHARGETIMER[] PROGMEM =        " Charge timer";
char const STR_SET_KWH_LIMIT[] PROGMEM =          " Charge limit";
char const STR_SET_KWH_COST[] PROGMEM =           " Cost per kWh";
char const STR_SET_SLEEPMODE[] PROGMEM =          " Sleep mode";
char const STR_SET_SLEEPMODE_TIME[] PROGMEM =     "show time";
char const STR_SET_SLEEPMODE_OFF[] PROGMEM =      "display off";
char const STR_SET_SLEEPMODE_DISABLED[] PROGMEM = "always on";
char const STR_SET_LCD_TYPE[] PROGMEM =           " LCD backlight";
char const STR_SET_TYPE_RGB[] PROGMEM =           "Color RGB";
char const STR_SET_TYPE_MONO[] PROGMEM =          "Monochrome";
char const STR_SET_RESET[] PROGMEM =              " Force restart";
char const STR_SET_EXIT[] PROGMEM =               " Exit settings";

// Faults                                         |1234567890123456|
char const STR_FAULT_POST_GFCI[] PROGMEM =        "GFI test failed";
char const STR_FAULT_STUCK_RELAY[] PROGMEM =      "Relay stuck";
char const STR_FAULT_GROUND_FAULT[] PROGMEM =     "Ground fault";
char const STR_FAULT_GFCI_TRIPPED[] PROGMEM =     "GFI tripped";
char const STR_FAULT_TEMPERATURE[] PROGMEM =      "High temperature";

// Stats                                          |1234567890123456|
char const STR_STATS_KWH[] PROGMEM =              "Charge totals:";
char const STR_STATS_WEEK[] PROGMEM =             "This week";
char const STR_STATS_MONTH[] PROGMEM =            "This month";
char const STR_STATS_YEAR[] PROGMEM =             "This year";
char const STR_STATS_TOTAL[] PROGMEM =            "Lifetime";

