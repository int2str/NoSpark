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
#include "event/loop.h"
#include "evse/settings.h"
#include "events.h"

#define SETTINGS_OFFSET     0x08
#define SETTINGS_MARKER     0xAEAE
#define SETTINGS_REVISION   0x07

namespace evse
{

Settings::Settings()
{
    defaults();
}

void Settings::defaults()
{
    marker = SETTINGS_MARKER;

    // Force upgrade
    revision = 0;
    upgrade();
}

void Settings::upgrade()
{
    // Rev 1
    if (revision < 1)
    {
        max_current = 16;
    }

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

    // Rev 5
    if (revision < 5)
    {
        kwh_currency = 0;
        kwh_cost = 0;
        ammeter_factor = 200;
        ammeter_offset = 0;
    }

    // Rev 6
    if (revision < 6)
    {
        lcd_type = 0;
    }

    // Rev 7
    if (revision < 7)
    {
        wh_total = kwh_total * 1000;
        wh_year = kwh_year * 1000;
        wh_month = kwh_month * 1000;
        wh_week = kwh_week * 1000;

        cost_total = kwh_total * kwh_cost;
        cost_year = kwh_year * kwh_cost;
        cost_month = kwh_month * kwh_cost;
        cost_week = kwh_week * kwh_cost;
    }

    revision = SETTINGS_REVISION;
}

void Settings::postLoad()
{
    devices::DS3231 &rtc = devices::DS3231::get();
    if (!rtc.isPresent())
        return;

    devices::TM t;
    rtc.read(t);

    if (t.year != (kwh_index & 0xFF))
        wh_year = cost_year = 0;
    if (t.month != ((kwh_index >> 8) & 0xF))
        wh_month = cost_month = 0;
    if (t.weekday < ((kwh_index >> 12) & 0xF))
        wh_week = cost_week = 0;
}

void Settings::preSave()
{
    devices::DS3231 &rtc = devices::DS3231::get();
    devices::TM t;

    rtc.read(t);
    kwh_index = (t.weekday << 12) | (t.month << 8) | t.year;
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
    event::Loop::post(event::Event(EVENT_SETTINGS_CHANGED));
}

}
