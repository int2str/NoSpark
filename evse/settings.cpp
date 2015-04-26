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

#include <avr/eeprom.h>

#include "settings.h"

#define SETTINGS_OFFSET     0x08
#define SETTINGS_MARKER     0xAEAE
#define SETTINGS_REVISION   0x02

namespace evse
{

Settings::Settings()
{
    defaults();
}

void Settings::defaults()
{
    marker = SETTINGS_MARKER;
    revision = SETTINGS_REVISION;

    // Rev 1
    max_current = 16;

    // Rev 2
    kwh_total = 0;
    kwh_year = 0;
    kwh_month = 0;
    kwh_week = 0;
    kwh_index = 0;
}

void Settings::upgrade()
{
    // Rev 2
    if (revision < 2)
    {
        kwh_total = 0;
        kwh_year = 0;
        kwh_month = 0;
        kwh_week = 0;
        kwh_index = 0;
    }

    revision = SETTINGS_REVISION;
}

void EepromSettings::load(Settings &settings)
{
    const void* addr = reinterpret_cast<void*>(SETTINGS_OFFSET);
    eeprom_read_block(&settings, addr, sizeof(Settings));

    if (settings.marker != SETTINGS_MARKER || settings.revision > SETTINGS_REVISION)
        settings.defaults();

    if (settings.revision < SETTINGS_REVISION)
        settings.upgrade();
}

void EepromSettings::save(const Settings &settings)
{
    void* addr = reinterpret_cast<void*>(SETTINGS_OFFSET);
    eeprom_write_block(&settings, addr, sizeof(Settings));
}

}
