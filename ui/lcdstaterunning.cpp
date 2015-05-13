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

#include <stdlib.h>
#include <string.h>

#include "board/j1772pilot.h"
#include "devices/lcd1602.h"
#include "devices/ds3231.h"
#include "evse/chargemonitor.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "utils/bcd.h"
#include "customcharacters.h"
#include "lcdstaterunning.h"
#include "strings.h"

#define LCD_COLUMNS 16

#define PAGE_TIMEOUT 5000

using board::J1772Pilot;
using devices::DS3231;
using devices::LCD16x2;
using evse::ChargeMonitor;
using evse::Settings;
using evse::EepromSettings;
using evse::State;
using stream::LcdStream;

namespace
{
    void write_time(LcdStream &lcd, uint8_t hh, uint8_t mm)
    {
        lcd << stream::PAD_ZERO << hh << ':' << stream::PAD_ZERO << mm;
    }

    void write_duration(LcdStream &lcd, const uint32_t ms)
    {
        if (!ms)
        {
            lcd << "--:--";
            return;
        }

        const uint32_t mins = ms / 1000 / 60;
        write_time(lcd, mins / 60, mins % 60);
    }

    void write_kwh(LcdStream &lcd, const uint32_t wh)
    {
        char buffer[10] = {0};
        ltoa(wh / 1000, buffer, 10);

        lcd << stream::Spaces(3 - strlen(buffer));
        lcd << buffer << '.' << static_cast<char>('0' + ((wh / 100) % 10)) << " kWh ";
    }

    uint8_t center_P(LcdStream &lcd, const char *str, const uint8_t offset = 0)
    {
        const uint8_t len = strlen_P(str) + offset;
        const uint8_t padding = (LCD_COLUMNS - len) / 2;
        lcd << stream::Spaces(padding) << stream::PGM << str << stream::Spaces(padding + 1);
        return padding;
    }
}

namespace ui
{

LcdStateRunning::LcdStateRunning(stream::LcdStream &lcd)
    : LcdState(lcd)
    , page(PAGE_DEFAULT)
    , display_state(PAGE_TIMEOUT)
{
    CustomCharacters::loadCustomChars(lcd.getLCD());
}

bool LcdStateRunning::draw()
{
    if (page == PAGE_DEFAULT)
        drawDefault();
    else
        drawKwhStats();

    return true;
}

void LcdStateRunning::drawDefault()
{
    const State &state = State::get();
    const ChargeMonitor &chargeMonitor = ChargeMonitor::get();
    DS3231 &rtc = DS3231::get();
    rtc.read();

    uint8_t amps = state.max_amps_limit;
    if (ChargeMonitor::get().isCharging())
        amps = chargeMonitor.chargeCurrent() / 1000;

    lcd.move(0,0);
    write_time(lcd, rtc.hour, rtc.minute);
    lcd << ' ' << stream::PAD_SPACE << rtc.readTemp() << static_cast<char>(0xDF);

    lcd << ' ' << static_cast<char>(CustomCharacters::SEPARATOR) << ' ';

    if (amps)
        lcd << stream::PAD_SPACE << amps;
    else
        lcd << "--";
    lcd << 'A';

    if (state.ready == State::SCHEDULED)
        lcd << static_cast<char>(CustomCharacters::HOURGLASS);
    else if (state.ready == State::KWH_LIMIT)
        lcd << static_cast<char>(CustomCharacters::BATTERY1);
    else
        lcd << ' ';

    lcd.move(0,1);

    switch (state.j1772)
    {
        case J1772Pilot::UNKNOWN:
        case J1772Pilot::STATE_E:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_ERROR_STATE);
            break;

        case J1772Pilot::STATE_A:
            lcd.setBacklight(LCD16x2::GREEN);
            if (chargeMonitor.chargeDuration() == 0)
            {
                center_P(lcd, STR_NOT_CONNECTED);
            } else {
                lcd << stream::PGM << STR_CHARGED;
                if (display_state.get())
                {
                    lcd << ' ';
                    write_duration(lcd, chargeMonitor.chargeDuration());
                    lcd << stream::Spaces(3);
                } else {
                    write_kwh(lcd, chargeMonitor.wattHours());
                }
            }
            break;

        case J1772Pilot::STATE_B:
            lcd.setBacklight(LCD16x2::GREEN);
            center_P(lcd, STR_CONNECTED);
            break;

        case J1772Pilot::STATE_C:
        {
            lcd.setBacklight(LCD16x2::CYAN);
            if (display_state.get())
                lcd << stream::PGM << STR_CHARGING;
            else
                write_kwh(lcd, chargeMonitor.wattHours());
            lcd << static_cast<char>(CustomCharacters::SEPARATOR);
            write_duration(lcd, chargeMonitor.chargeDuration());
            break;
        }

        case J1772Pilot::STATE_D:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_VENT_REQUIRED);
            break;

        case J1772Pilot::DIODE_CHECK_FAILED:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_DIODE_CHECK_FAILED);
            break;

        case J1772Pilot::NOT_READY:
        case J1772Pilot::IMPLAUSIBLE:
            break;
    }
}

void LcdStateRunning::drawKwhStats()
{
    Settings settings;
    EepromSettings::load(settings);

    lcd.setBacklight(LCD16x2::WHITE);

    lcd.move(0,0);
    lcd << stream::PGM << (STR_STATS_KWH);

    lcd.move(0,1);
    uint16_t *p = 0;
    switch (page)
    {
        case PAGE_KWH_WEEK:
            lcd << stream::PGM << STR_STATS_WEEK;
            p = &settings.kwh_week;
            break;

        case PAGE_KWH_MONTH:
            lcd << stream::PGM << STR_STATS_MONTH;
            p = &settings.kwh_month;
            break;

        case PAGE_KWH_YEAR:
            lcd << stream::PGM << STR_STATS_YEAR;
            p = &settings.kwh_year;
            break;

        case PAGE_KWH_TOTAL:
            lcd << stream::PGM << STR_STATS_TOTAL;
            p = &settings.kwh_total;
            break;

        default:
            break;
    }

    char buffer[10] = {0};
    ltoa(p ? *p : 0, buffer, 10);

    lcd << ": " << buffer << stream::Spaces(5);
}

void LcdStateRunning::select()
{
    lcd.clear();
    if (++page == PAGE_MAX)
        page = PAGE_DEFAULT;
}

}
