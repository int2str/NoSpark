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

#include <util/delay.h>
#include <stdlib.h>

#include "board/ammeter.h"
#include "board/j1772status.h"
#include "devices/ds3231.h"
#include "evse/state.h"
#include "system/timer.h"
#include "utils/movingaverage.h"
#include "customcharacters.h"
#include "lcdstaterunning.h"
#include "strings.h"

#define LCD_COLUMNS 16

using board::Ammeter;
using board::J1772Status;
using devices::DS3231;
using devices::LCD16x2;
using evse::State;
using utils::MovingAverage;

namespace
{
    void write_bcd(LCD16x2 &lcd, const uint8_t bcd)
    {
        lcd.write('0' + (bcd >> 4));
        lcd.write('0' + (bcd & 0x0F));
    }

    void write_dec(LCD16x2 &lcd, const uint8_t dec)
    {
        if (dec < 10)
            lcd.write('0');
        else
            lcd.write('0' + (dec / 10));
        lcd.write('0' + (dec % 10));
    }

    void write_temp(LCD16x2 &lcd, const uint8_t temp)
    {
        if (temp < 10)
            lcd.write(' ');
        else
            lcd.write('0' + (temp / 10));
        lcd.write('0' + (temp % 10));
        lcd.write(0xDF);
    }

    void write_time(LCD16x2 &lcd, DS3231 &rtc)
    {
        uint8_t buffer[7] = {0};
        rtc.readRaw(buffer, 7);

        write_bcd(lcd, buffer[2]);
        lcd.write(':');
        write_bcd(lcd, buffer[1]);
    }

    void write_time(LCD16x2 &lcd, const uint32_t ms)
    {
        if (!ms)
        {
            lcd.write("--:--");
            return;
        }

        const uint32_t mins = ms / 60000;
        write_dec(lcd, mins / 60);
        lcd.write(":");
        write_dec(lcd, mins % 60);
    }

    void spaces(LCD16x2 &lcd, const uint8_t num)
    {
        for(uint8_t i = 0; i != num; ++i)
            lcd.write(' ');
    }

    uint8_t center_P(LCD16x2 &lcd, const char *str, const uint8_t offset = 0)
    {
        const uint8_t len = strlen_P(str) + offset;
        const uint8_t padding = (LCD_COLUMNS - len) / 2;
        spaces(lcd, padding);
        lcd.write_P(str);
        spaces(lcd, padding + 1);
        return padding;
    }
}

namespace ui
{

LcdStateRunning::LcdStateRunning(devices::LCD16x2 &lcd)
    : LcdState(lcd)
{
}

bool LcdStateRunning::draw()
{
    DS3231& rtc = DS3231::get();
    const State& state = State::get();
    uint8_t amps = state.max_amps_limit;

    // TODO:
    // The current sensing and averaging is here
    // temporarily. Ultimately all charge time/energy
    // monitoring will go in it's own module.
    static MovingAverage<uint32_t, 25> ma;

    if (state.j1772 == J1772Status::STATE_C)
    {
        ma.push(Ammeter::sample());
        amps = ma.get() / 1000;
    } else {
        ma.clear();
    }

    lcd.move(0,0);
    write_time(lcd, rtc);
    lcd.write(' ');
    write_temp(lcd, rtc.readTemp());

    lcd.write(' ');
    lcd.write(CUSTOM_CHAR_SEPARATOR);
    lcd.write(' ');

    if (amps)
    {
        lcd.write(amps >= 10 ? '0' + amps / 10 : ' ');
        lcd.write('0' + amps % 10);
    } else {
        lcd.write("--");
    }
    lcd.write('A');

    lcd.move(0,1);

    switch (state.j1772)
    {
        case J1772Status::UNKNOWN:
        case J1772Status::STATE_E:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_ERROR_STATE);
            break;

        case J1772Status::STATE_A:
            lcd.setBacklight(LCD16x2::GREEN);
            center_P(lcd, STR_NOT_CONNECTED);
            break;

        case J1772Status::STATE_B:
            lcd.setBacklight(LCD16x2::GREEN);
            if (state.charge_start_time == 0)
            {
                center_P(lcd, STR_CONNECTED);
            } else {
                const uint8_t offset = center_P(lcd, STR_CHARGED, 5) + 5;
                lcd.move(LCD_COLUMNS - offset, 1);
                write_time(lcd, state.charge_stop_time - state.charge_start_time);
            }
            break;

        case J1772Status::STATE_C:
            lcd.setBacklight(LCD16x2::CYAN);
            lcd.write_P(STR_CHARGING);
            lcd.write(CUSTOM_CHAR_SEPARATOR);
            write_time(lcd, system::Timer::millis() - state.charge_start_time);
            break;

        case J1772Status::STATE_D:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_VENT_REQUIRED);
            break;

        case J1772Status::DIODE_CHECK_FAILED:
            lcd.setBacklight(LCD16x2::RED);
            center_P(lcd, STR_DIODE_CHECK_FAILED);
            break;

        case J1772Status::NOT_READY:
        case J1772Status::IMPLAUSIBLE:
            break;
    }

    return true;
}

}
