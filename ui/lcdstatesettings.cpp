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
#include "customcharacters.h"
#include "events.h"
#include "lcdstatesettings.h"
#include "strings.h"

#define SETTINGS_TIMEOUT    10000

using devices::DS3231;
using devices::LCD16x2;
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
    , option(0)
    , adjusting(false)
    , lastAction(0)
    , lastUpdate(0)
    , uiState(0)
    , pageHandlers {
        &LcdStateSettings::pageSetTime
      , &LcdStateSettings::pageSetDate
      , &LcdStateSettings::pageSetCurrent
      , &LcdStateSettings::pageReset
      , &LcdStateSettings::pageExit
    }
{
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
    // Draw screen

    lcd.move(0,0);
    lcd.write(CUSTOM_CHAR_CLOCK);
    lcd.write_P(STR_SET_CLOCK);

    lcd.move(2, 1);
    if (!adjusting || uiState)
    {
        uint8_t buffer[7] = {0};
        DS3231::get().readRaw(buffer, 7);

        write_digits(lcd, buffer[2]);
        lcd.write(':');
        write_digits(lcd, buffer[1]);
    } else {
        lcd.write("             ");
    }

    updateUIState();
    return true;
}

bool LcdStateSettings::pageSetDate()
{
    // Draw screen

    lcd.move(0,0);
    lcd.write(CUSTOM_CHAR_CALENDAR);
    lcd.write_P(STR_SET_DATE);

    lcd.move(2, 1);
    if (!adjusting || uiState)
    {
        uint8_t buffer[7] = {0};
        DS3231::get().readRaw(buffer, 7);

        write_digits(lcd, buffer[4]);
        lcd.write('.');
        write_digits(lcd, buffer[5]);
        lcd.write(".20");
        write_digits(lcd, buffer[6]);
    } else {
        lcd.write("             ");
    }

    updateUIState();
    return true;
}

bool LcdStateSettings::pageSetCurrent()
{
    const uint8_t currents[] = {10, 16, 20, 24, 30, 35, 40, 45, 50};

    // Initialize amps
    if (option == 0)
        option = State::get().max_amps_target;

    // Save new state if we're done adjusting
    if (!adjusting && option != State::get().max_amps_target)
    {
        State::get().max_amps_target = option;
        event::Loop::post(event::Event(EVENT_MAX_AMPS_CHANGED, option));
    }

    // Snap to currents above if we're still adjusting
    if (adjusting)
    {
        if (option > currents[sizeof(currents) - 1])
            option = currents[0];

        // Snap
        uint8_t i = 0;
        while (currents[i] < option)
            ++i;
        option = currents[i];
    }

    // Draw screen, flashing option while adjusting

    lcd.move(0, 0);
    lcd.write(CUSTOM_CHAR_BOLT);
    lcd.write_P(STR_SET_CURRENT);

    lcd.move(2, 1);
    if (!adjusting || uiState)
    {
        char buffer[4] = {0};
        utoa(option, buffer, 10);
        lcd.write(buffer);
        lcd.write('A');
    } else {
        lcd.write("             ");
    }

    updateUIState();
    return true;
}

bool LcdStateSettings::pageReset()
{
    lcd.move(0, 0);
    lcd.write('!');
    lcd.write_P(STR_SET_RESET);
    if (adjusting)
        Watchdog::forceRestart();
    return true;
}

bool LcdStateSettings::pageExit()
{
    lcd.move(0, 0);
    lcd.write(0x7F); // <- back arrow
    lcd.write_P(STR_SET_EXIT);
    return !adjusting;
}

void LcdStateSettings::select()
{
    if (!adjusting)
    {
        lcd.clear();
        ++page;
        option = 0;
    } else {
        ++option;
    }

    resetTimeout();
}

void LcdStateSettings::advance()
{
    uiState = 0;
    adjusting = !adjusting;
    resetTimeout();
}

bool LcdStateSettings::timedOut()
{
    if (lastAction == 0)
        resetTimeout();
    return (Timer::millis() - lastAction) > SETTINGS_TIMEOUT;
}

void LcdStateSettings::resetTimeout()
{
    lastAction = Timer::millis();
}

void LcdStateSettings::updateUIState()
{
    if (Timer::millis() - lastUpdate > 500)
    {
        uiState = !uiState;
        lastUpdate = Timer::millis();
    }
}

}