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
#define SETTINGS_REVISION   0x01

namespace evse
{

void EepromSettings::load(Settings &settings)
{
    const void* addr = reinterpret_cast<void*>(SETTINGS_OFFSET);
    eeprom_read_block(&settings, addr, sizeof(Settings));

    if (settings.marker != SETTINGS_MARKER || settings.revision != SETTINGS_REVISION)
    {
        settings.marker = SETTINGS_MARKER;
        settings.revision = SETTINGS_REVISION;
        settings.defaults();
        EepromSettings::save(settings);
    }
}

void EepromSettings::save(const Settings &settings)
{
    void* addr = reinterpret_cast<void*>(SETTINGS_OFFSET);
    eeprom_write_block(&settings, addr, sizeof(Settings));
}

}
