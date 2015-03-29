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

#include "board/j1772status.h"
#include "devices/ds3231.h"
#include "evse/state.h"
#include "customcharacters.h"
#include "lcdstaterunning.h"
#include "strings.h"

#define LCD_COLUMNS 16

using board::J1772Status;
using devices::DS3231;
using devices::LCD16x2;
using evse::State;

namespace
{
    void write_digits(LCD16x2 &lcd, const uint8_t d)
    {
        lcd.write('0' + (d >> 4));
        lcd.write('0' + (d & 0x0F));
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

        write_digits(lcd, buffer[2]);
        lcd.write(':');
        write_digits(lcd, buffer[1]);
    }

    void spaces(LCD16x2 &lcd, const uint8_t num)
    {
        for(uint8_t i = 0; i != num; ++i)
            lcd.write(' ');
    }

    void center_P(LCD16x2 &lcd, const char *str)
    {
        const uint8_t len = strlen_P(str);
        spaces(lcd, (LCD_COLUMNS - len) / 2);
        lcd.write_P(str);
        spaces(lcd, (LCD_COLUMNS - len) / 2 + 1);
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
    const uint8_t max_amps = state.max_amps;

    lcd.move(0,0);
    write_time(lcd, rtc);
    lcd.write(' ');
    write_temp(lcd, rtc.readTemp());

    lcd.write(' ');
    lcd.write(CUSTOM_CHAR_SEPARATOR);
    lcd.write(' ');
    lcd.write(max_amps >= 10 ? '0' + max_amps / 10 : ' ');
    lcd.write('0' + max_amps % 10);
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
            center_P(lcd, STR_CONNECTED);
            break;

        case J1772Status::STATE_C:
            lcd.setBacklight(LCD16x2::CYAN);
            lcd.write_P(STR_CHARGING);
            lcd.write("  ");
            lcd.write(CUSTOM_CHAR_SEPARATOR);
            lcd.write("00:00");
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
