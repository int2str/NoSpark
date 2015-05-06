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

#include <stdint.h>

namespace evse
{

class Settings
{
public:
    uint16_t marker;
    uint8_t revision;

    // Rev 1
    uint8_t max_current;

    // Rev 2
    uint16_t kwh_total;
    uint16_t kwh_year;
    uint16_t kwh_month;
    uint16_t kwh_week;
    uint16_t kwh_index;

    // Rev 3
    uint8_t kwh_limit;
    uint8_t sleep_mode;

    Settings();

    void defaults();
    void upgrade();
    void postLoad();
    void preSave();
};


class EepromSettings
{
public:
    static void load(Settings &settings);
    static void save(Settings &settings);
};

}
