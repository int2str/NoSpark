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

#include "evse/state.h"
#include "system/timer.h"
#include "events.h"
#include "lcdconsole.h"
#include "lcdstatebootup.h"
#include "lcdstateerror.h"
#include "lcdstaterunning.h"
#include "lcdstatesettings.h"
#include "strings.h"

using devices::LCD16x2;
using evse::State;
using system::Timer;

namespace ui
{

LcdConsole& LcdConsole::init()
{
    static LcdConsole console;
    return console;
}

LcdConsole::LcdConsole()
    : inSettings(false)
    , lcdState(new LcdStateBootup())
{
    const uint8_t separator[] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
    lcd.createChar(1, separator, 8);
}

void LcdConsole::setState(LcdState *newState)
{
    if (lcdState != 0)
        delete lcdState;

    lcd.clear();
    lcdState = newState;
    if (lcdState)
        lcdState->draw(lcd);
}


void LcdConsole::update()
{
    const bool keepState = lcdState->draw(lcd);
    if (!keepState && inSettings)
    {
        inSettings = false;
        setState(new LcdStateRunning());
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
                setState(new LcdStateRunning());
            else if (event.param == State::FAULT)
                setState(new LcdStateError());
            break;

        case EVENT_KEYHOLD:
            if (!inSettings)
            {
                inSettings = true;
                setState(new LcdStateSettings());
            } else {
                lcdState->advance();
            }
            break;

        case EVENT_KEYUP:
            lcdState->select();
            break;
    }
}

}
