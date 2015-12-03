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
#include "stream/time.h"
#include "utils/bcd.h"
#include "customcharacters.h"
#include "lcdstaterunning.h"
#include "strings.h"

#define LCD_COLUMNS 16

#define DEGREE_SYMBOL  static_cast<char>(0xDF)

#define PAGE_TIMEOUT 5000

using board::J1772Pilot;
using devices::DS3231;
using devices::TM;
using devices::LCD16x2;
using evse::ChargeMonitor;
using evse::Settings;
using evse::EepromSettings;
using evse::State;
using stream::LcdStream;
using stream::OutputStream;

namespace
{
    void write_duration(OutputStream &lcd, const uint32_t ms)
    {
        if (!ms)
        {
            lcd << "--:--";
            return;
        }

        const uint32_t mins = ms / 1000 / 60;
        lcd << stream::Time(mins / 60, mins % 60);
    }

    void write_kwh(OutputStream &lcd, const uint32_t wh)
    {
        char buffer[10] = {0};
        ltoa(wh / 1000, buffer, 10);

        lcd << buffer << '.' << static_cast<char>('0' + ((wh / 100) % 10)) << " kWh";
    }

    void write_cost(OutputStream &lcd, const uint8_t currency, uint32_t cost)
    {
        if (cost == 0)
            return;

        const char currencies[3] = {'$', ui::CustomCharacters::EURO, '\\'}; // Backslash = Yen

        char buffer[10] = {0};
        ltoa(cost / 100, buffer, 10);
        cost %= 100;

        lcd << " / "
          << static_cast<char>(currencies[currency]) << buffer << "."
          << static_cast<char>('0' + cost / 10)
          << static_cast<char>('0' + cost % 10);
    }

    uint8_t center_P(OutputStream &lcd, const char *str, const uint8_t offset = 0)
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
    , scrolling_text(40, 11)
{
    CustomCharacters::loadCustomChars(lcd.getLCD());
    EepromSettings::load(settings);
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
    const uint32_t wh = chargeMonitor.wattHours();
    const uint32_t cost = wh * settings.kwh_cost / 1000;
    const uint32_t duration = chargeMonitor.chargeDuration();

    uint8_t amps = state.max_amps_limit;
    if (chargeMonitor.isCharging())
        amps = chargeMonitor.chargeCurrent() / 1000;

    lcd.move(0,0);
        
    DS3231 &rtc = DS3231::get();
    if (rtc.isPresent())
    {
        TM t;
        rtc.read(t);
        lcd << stream::Time(t.hour, t.minute)
          << stream::PAD_SPACE << rtc.readTemp() << DEGREE_SYMBOL;
    } else {
        switch (state.j1772)
        {
            case J1772Pilot::STATE_A:
                lcd << stream::PGM << STR_STATE_READY;
                break;

            case J1772Pilot::STATE_B:
                lcd << stream::PGM << STR_STATE_CONNECTED;
                break;

            case J1772Pilot::STATE_C:
                lcd << stream::PGM << STR_STATE_CHARGING;
                break;

            default:
                lcd << stream::PGM << STR_STATE_ERROR;
                break;
        }
    }

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
    scrolling_text.clear();

    switch (state.j1772)
    {
        case J1772Pilot::UNKNOWN:
            break;

        case J1772Pilot::STATE_E:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_STATE_ERROR);
            break;

        case J1772Pilot::STATE_A:
            lcd.setBacklight(LCD16x2::GREEN);
            if ((duration / 60000 == 0) &&
                (wh / 100 == 0) &&
                (cost == 0))
            {
                center_P(lcd, STR_NOT_CONNECTED);
            } else {
                scrolling_text.setWidth(LCD_COLUMNS);
                scrolling_text << stream::PGM << STR_CHARGED << " ";
                write_duration(scrolling_text, duration);
                scrolling_text << " / ";
                write_kwh(scrolling_text, wh);
                write_cost(scrolling_text, settings.kwh_currency, cost);
                scrolling_text >> lcd;
            }
            break;

        case J1772Pilot::STATE_B:
            lcd.setBacklight(LCD16x2::YELLOW);
            center_P(lcd, STR_CONNECTED);
            break;

        case J1772Pilot::STATE_C:
        {
            lcd.setBacklight(LCD16x2::CYAN);

            scrolling_text.setWidth(10);
            if (rtc.isPresent())
                scrolling_text << stream::PGM << STR_CHARGING << " ";
            write_kwh(scrolling_text, wh);
            write_cost(scrolling_text, settings.kwh_currency, cost);
            scrolling_text >> lcd;

            lcd << static_cast<char>(CustomCharacters::SEPARATOR);
            write_duration(lcd, duration);
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
    lcd.setBacklight(LCD16x2::WHITE);

    lcd.move(0,0);
    lcd << stream::PGM << (STR_STATS_KWH);

    lcd.move(0,1);
    scrolling_text.clear();
    scrolling_text.setWidth(16);

    uint32_t power = 0;
    uint32_t cost = 0;
    switch (page)
    {
        case PAGE_KWH_WEEK:
            scrolling_text << stream::PGM << STR_STATS_WEEK;
            power = settings.wh_week;
            cost = settings.cost_week;
            break;

        case PAGE_KWH_MONTH:
            scrolling_text << stream::PGM << STR_STATS_MONTH;
            power = settings.wh_month;
            cost = settings.cost_month;
            break;

        case PAGE_KWH_YEAR:
            scrolling_text << stream::PGM << STR_STATS_YEAR;
            power = settings.wh_year;
            cost = settings.cost_year;
            break;

        case PAGE_KWH_TOTAL:
            scrolling_text << stream::PGM << STR_STATS_TOTAL;
            power = settings.wh_total;
            cost = settings.cost_total;
            break;

        default:
            break;
    }

    scrolling_text << " ";
    write_kwh(scrolling_text, power);
    write_cost(scrolling_text, settings.kwh_currency, cost);
    scrolling_text >> lcd;
}

void LcdStateRunning::select()
{
    lcd.clear();
    if (++page == PAGE_MAX)
        page = PAGE_DEFAULT;

    // If we're about to display KWH stats, refresh the settings first
    if (page != PAGE_DEFAULT)
        EepromSettings::load(settings);
}

}
