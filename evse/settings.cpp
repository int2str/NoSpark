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

#include "devices/ds3231.h"
#include "settings.h"

#define SETTINGS_OFFSET     0x08
#define SETTINGS_MARKER     0xAEAE
#define SETTINGS_REVISION   0x04

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

    // Rev 3
    kwh_limit = 0;
    sleep_mode = 0;

    // Rev 4
    charge_timer_enabled = 0;
    charge_timer_t1 = (23 << 8);
    charge_timer_t2 = (7 << 8);
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

    // Rev 3
    if (revision < 3)
    {
        kwh_limit = 0;
        sleep_mode = 0;
    }

    // Rev 4
    if (revision < 4)
    {
        charge_timer_enabled = 0;
        charge_timer_t1 = (23 << 8);
        charge_timer_t2 = (7 << 8);
    }

    revision = SETTINGS_REVISION;
}

void Settings::postLoad()
{
    devices::DS3231 &rtc = devices::DS3231::get();
    rtc.read();

    if (rtc.year != (kwh_index & 0xFF))
        kwh_year = 0;
    if (rtc.month != ((kwh_index >> 8) & 0xF))
        kwh_month = 0;
    if (rtc.weekday < ((kwh_index >> 12) & 0xF))
        kwh_week = 0;
}

void Settings::preSave()
{
    devices::DS3231 &rtc = devices::DS3231::get();
    rtc.read();
    kwh_index = (rtc.weekday << 12) | (rtc.month << 8) | rtc.year;
}

void EepromSettings::load(Settings &settings)
{
    const void* addr = reinterpret_cast<void*>(SETTINGS_OFFSET);
    eeprom_read_block(&settings, addr, sizeof(Settings));

    if (settings.marker != SETTINGS_MARKER || settings.revision > SETTINGS_REVISION)
        settings.defaults();

    if (settings.revision < SETTINGS_REVISION)
        settings.upgrade();

    settings.postLoad();
}

void EepromSettings::save(Settings &settings)
{
    void* addr = reinterpret_cast<void*>(SETTINGS_OFFSET);
    settings.preSave();
    eeprom_write_block(&settings, addr, sizeof(Settings));
}

}
