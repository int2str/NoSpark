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

#include <avr/io.h>
#include <util/delay.h>

#include "board/j1772status.h"
#include "evse/state.h"
#include "system/timer.h"
#include "customcharacters.h"
#include "events.h"
#include "lcdconsole.h"
#include "lcdstatebootup.h"
#include "lcdstateerror.h"
#include "lcdstaterunning.h"
#include "lcdstatesettings.h"
#include "strings.h"

#define SLEEP_DELAY_MS (900l * 1000l)

using board::J1772Status;
using devices::LCD16x2;
using evse::State;
using system::Timer;

namespace
{
    void loadCustomCharacters(LCD16x2 &lcd)
    {
        for (uint8_t i=0; i != NUM_CUSTOM_CHARS; ++i)
            lcd.createChar_P(i, &ui::CUSTOM_CHAR_MAP[i][0], 8);
    }
}

namespace ui
{

LcdConsole& LcdConsole::init()
{
    static LcdConsole console;
    return console;
}

LcdConsole::LcdConsole()
    : in_settings(false)
    , sleeping(false)
    , last_event(Timer::millis())
    , lcdState(new LcdStateBootup(lcd))
{
    loadCustomCharacters(lcd);
}

void LcdConsole::setState(LcdState *newState)
{
    if (lcdState != 0)
        delete lcdState;

    lcd.clear();
    lcdState = newState;

    if (!sleeping)
        lcdState->draw();
}


void LcdConsole::update()
{
    if (sleeping)
        return;

    const bool keepState = lcdState->draw();
    if (!keepState && in_settings)
    {
        in_settings = false;
        if (State::get().fault == State::NOTHING_WRONG)
            setState(new LcdStateRunning(lcd));
        else
            setState(new LcdStateError(lcd));
    }
}

void LcdConsole::onEvent(const event::Event &event)
{
    switch (event.id)
    {
        case EVENT_UPDATE:
            update();
            break;

        case EVENT_CONTROLLER_STATE:
            if (event.param == State::RUNNING)
                setState(new LcdStateRunning(lcd));
            else if (event.param == State::FAULT)
                setState(new LcdStateError(lcd));
            break;

        case EVENT_KEYHOLD:
            if (!sleeping)
            {
                if (!in_settings)
                {
                    in_settings = true;
                    setState(new LcdStateSettings(lcd));
                } else {
                    lcdState->advance();
                }
            }
            break;

        case EVENT_KEYUP:
            if (!sleeping)
                lcdState->select();
            break;
    }

    // Check for "sleep" mode...
    if (event.id == EVENT_UPDATE && State::get().j1772 == J1772Status::STATE_A)
    {
        if (!sleeping && (Timer::millis() - last_event) > SLEEP_DELAY_MS)
        {
            lcd.setBacklight(LCD16x2::OFF);
            sleeping = 1;
        }

    } else if (event.id != EVENT_KEYDOWN) {
        last_event = Timer::millis();
        sleeping = 0;
    }
}

}
