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

#include "devices/ds3231.h"
#include "event/loop.h"
#include "evse/state.h"
#include "system/timer.h"
#include "system/watchdog.h"
#include "utils/bcd.h"
#include "utils/math.h"
#include "customcharacters.h"
#include "events.h"
#include "lcdstatesettings.h"
#include "strings.h"

#define SETTINGS_TIMEOUT    10000
#define BLINK_TIMEOUT       500

#define KWH_LIMIT_MAX       15

#define UNINITIALIZED       0xFF

#define NOT_ADJUSTING       0x00
#define ADJUST_SINGLE       0x01

#define ADJUST_HH           0x01
#define ADJUST_MM           0x02

#define ADJUST_DD           0x01
#define ADJUST_MM           0x02
#define ADJUST_YY           0x03

using devices::DS3231;
using devices::LCD16x2;
using evse::EepromSettings;
using evse::State;
using system::Timer;
using system::Watchdog;

namespace
{
    void write_digits(devices::LCD16x2 &lcd, const uint8_t bcd)
    {
        lcd.write('0' + (bcd >> 4));
        lcd.write('0' + (bcd & 0x0F));
    }
}

namespace ui
{

LcdStateSettings::LcdStateSettings(devices::LCD16x2 &lcd)
    : LcdState(lcd)
    , page(0)
    , option(NOT_ADJUSTING)
    , value(UNINITIALIZED)
    , last_action(0)
    , blink_state(BLINK_TIMEOUT)
    , pageHandlers {
        &LcdStateSettings::pageSetTime
      , &LcdStateSettings::pageSetDate
      , &LcdStateSettings::pageSetCurrent
      , &LcdStateSettings::pageKwhLimit
      , &LcdStateSettings::pageSleepmode
      , &LcdStateSettings::pageReset
      , &LcdStateSettings::pageExit
    }
{
    CustomCharacters::loadCustomChars(lcd);
    EepromSettings::load(settings);
}

bool LcdStateSettings::draw()
{
    if (timedOut())
        return false;

    lcd.setBacklight(LCD16x2::WHITE);

    if (page >= SETTINGS_PAGES)
        page = 0;

    return (this->*pageHandlers[page])();
}

bool LcdStateSettings::pageSetTime()
{
    lcd.move(0,0);
    lcd.write(CustomCharacters::CLOCK);
    lcd.write_P(STR_SET_CLOCK);
    lcd.move(2, 1);

    static uint8_t time_buffer[8] = {0};
    if (option > ADJUST_MM)
    {
        DS3231::get().writeRaw(time_buffer, 8);
        option = NOT_ADJUSTING;
    }

    if (option == NOT_ADJUSTING)
        DS3231::get().readRaw(time_buffer, 8);

    if (value == UNINITIALIZED)
        value = 0;

    const uint8_t hh = utils::bcd2dec(time_buffer[3]);
    if (option == ADJUST_HH)
        time_buffer[3] = utils::dec2bcd((hh + value) % 24);

    const uint8_t mm = utils::bcd2dec(time_buffer[2]);
    if (option == ADJUST_MM)
        time_buffer[2] = utils::dec2bcd((mm + value) % 60);

    write_digits(lcd, time_buffer[3]);
    lcd.write(':');
    write_digits(lcd, time_buffer[2]);

    if (option != NOT_ADJUSTING && !blink_state.get())
    {
        const uint8_t offset[2] = {2, 5};
        lcd.move(offset[option - 1], 1);
        lcd.write("  ");
    }

    value = 0;
    return true;
}

bool LcdStateSettings::pageSetDate()
{
    lcd.move(0,0);
    lcd.write(CustomCharacters::CALENDAR);
    lcd.write_P(STR_SET_DATE);
    lcd.move(2, 1);

    static uint8_t time_buffer[8] = {0};
    if (option > ADJUST_YY)
    {
        DS3231::get().writeRaw(time_buffer, 8);
        option = NOT_ADJUSTING;
    }

    if (option == NOT_ADJUSTING)
        DS3231::get().readRaw(time_buffer, 8);

    if (value == UNINITIALIZED)
        value = 0;

    const uint8_t dd = utils::bcd2dec(time_buffer[5]);
    if (option == ADJUST_DD)
        time_buffer[5] = utils::dec2bcd(utils::max((dd + value) % 32, 1));

    const uint8_t mm = utils::bcd2dec(time_buffer[6]);
    if (option == ADJUST_MM)
        time_buffer[6] = utils::dec2bcd(utils::max((mm + value) % 13, 1));

    const uint8_t yy = utils::bcd2dec(time_buffer[7]);
    if (option == ADJUST_YY)
        time_buffer[7] = utils::dec2bcd((yy + value) % 30); // <-- Year 2030 issue :)

    write_digits(lcd, time_buffer[5]);
    lcd.write('.');
    write_digits(lcd, time_buffer[6]);
    lcd.write(".20");
    write_digits(lcd, time_buffer[7]);

    if (option != NOT_ADJUSTING && !blink_state.get())
    {
        const uint8_t offset[3] = {2, 5, 8};
        lcd.move(offset[option - 1], 1);
        lcd.write(option == ADJUST_YY ? "    " : "  ");
    }

    value = 0;
    return true;
}

bool LcdStateSettings::pageSetCurrent()
{
    const uint8_t currents[] = {10, 16, 20, 24, 30, 35, 40, 45, 50};

    // Initialize amps
    if (value == UNINITIALIZED)
        value = State::get().max_amps_target;

    // Save new state if we're done adjusting
    if (option > ADJUST_SINGLE)
    {
        if (value != State::get().max_amps_target)
        {
            settings.max_current = value;
            EepromSettings::save(settings);

            State::get().max_amps_target = value;
            event::Loop::post(event::Event(EVENT_MAX_AMPS_CHANGED, value));
        }
        option = NOT_ADJUSTING;
    }

    // Snap to currents above if we're still adjusting
    if (option == ADJUST_SINGLE)
    {
        if (value > currents[sizeof(currents) - 1])
            value = currents[0];

        // Snap
        uint8_t i = 0;
        while (currents[i] < value)
            ++i;
        value = currents[i];
    }

    // Draw screen, flashing value while adjusting

    lcd.move(0, 0);
    lcd.write(CustomCharacters::BOLT);
    lcd.write_P(STR_SET_CURRENT);

    lcd.move(2, 1);
    if (option == NOT_ADJUSTING || blink_state.get())
    {
        char buffer[4] = {0};
        utoa(value, buffer, 10);
        lcd.write(buffer);
        lcd.write('A');
    } else {
        lcd.write("             ");
    }

    return true;
}

bool LcdStateSettings::pageKwhLimit()
{
    // Save new state if we're done adjusting
    if (option > ADJUST_SINGLE)
    {
        settings.kwh_limit = value;
        EepromSettings::save(settings);
        // TODO: Signal controller to enforce limit
        option = NOT_ADJUSTING;
    }

    if (value == UNINITIALIZED)
        value = 0;

    if (option == ADJUST_SINGLE)
    {
        if (value > KWH_LIMIT_MAX)
            value = 0;
    }

    // Draw screen, flashing value while adjusting

    lcd.move(0, 0);
    lcd.write(CustomCharacters::BATTERY1);
    lcd.write_P(STR_SET_KWH_LIMIT);

    lcd.move(2, 1);
    if (option == NOT_ADJUSTING || blink_state.get())
    {
        if (value == 0)
        {
            lcd.write_P(STR_OFF);
        } else {
            char buffer[4] = {0};
            utoa(value, buffer, 10);
            lcd.write(buffer);
            lcd.write(" kWh  ");
        }
    } else {
        lcd.write("             ");
    }

    return true;
}

bool LcdStateSettings::pageSleepmode()
{
    // Save new state if we're done adjusting
    if (option > ADJUST_SINGLE)
    {
        settings.sleep_mode = value;
        EepromSettings::save(settings);
        option = NOT_ADJUSTING;
    }

    if (value == UNINITIALIZED)
        value = settings.sleep_mode;

    // Draw screen, flashing value while adjusting

    lcd.move(0, 0);
    lcd.write(CustomCharacters::ZZ);
    lcd.write_P(STR_SET_SLEEPMODE);

    lcd.move(2, 1);
    if (option == NOT_ADJUSTING || blink_state.get())
    {
        switch (value)
        {
            default:
                value = 0;
                // fall-through intended
            case 0:
                lcd.write_P(STR_SET_SLEEPMODE_TIME);
                break;
            case 1:
                lcd.write_P(STR_SET_SLEEPMODE_OFF);
                break;
            case 2:
                lcd.write_P(STR_SET_SLEEPMODE_DISABLED);
                break;
        }
    } else {
        lcd.write("             ");
    }

    return true;
}

bool LcdStateSettings::pageReset()
{
    lcd.move(0, 0);
    lcd.write('!');
    lcd.write_P(STR_SET_RESET);
    if (option != NOT_ADJUSTING)
        Watchdog::forceRestart();
    return true;
}

bool LcdStateSettings::pageExit()
{
    lcd.move(0, 0);
    lcd.write(0x7F); // <- back arrow
    lcd.write_P(STR_SET_EXIT);
    return (option == NOT_ADJUSTING);
}

void LcdStateSettings::select()
{
    if (option == NOT_ADJUSTING)
    {
        ++page;
        value = UNINITIALIZED;
        lcd.clear();
    } else {
        ++value;
    }

    resetTimeout();
}

void LcdStateSettings::advance()
{
    ++option;
    resetTimeout();
}

bool LcdStateSettings::timedOut()
{
    if (last_action == 0)
        resetTimeout();
    return (Timer::millis() - last_action) > SETTINGS_TIMEOUT;
}

void LcdStateSettings::resetTimeout()
{
    last_action = Timer::millis();
}

}
