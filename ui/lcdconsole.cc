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

#include "board/j1772pilot.h"
#include "events.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "system/timer.h"
#include "ui/lcdconsole.h"
#include "ui/lcdstatebootup.h"
#include "ui/lcdstateerror.h"
#include "ui/lcdstaterunning.h"
#include "ui/lcdstatesettings.h"
#include "ui/lcdstatesleeping.h"
#include "ui/strings.h"

#define SLEEP_DELAY_MS (900l * 1000l)

using nospark::board::J1772Pilot;
using nospark::devices::LCD16x2;
using nospark::evse::EepromSettings;
using nospark::evse::Settings;
using nospark::evse::State;
using nospark::system::Timer;

namespace {
bool isWakeEvent(const uint8_t event_id) {
  return event_id != EVENT_UPDATE_CHARGE_TIMER && event_id != EVENT_KEYDOWN &&
         event_id != EVENT_REQUEST_SLEEP;
}

void updateBacklightType(LCD16x2 &lcd) {
  Settings settings;
  EepromSettings::load(settings);
  lcd.setBacklightType(settings.lcd_type == 0 ? LCD16x2::RGB
                                              : LCD16x2::MONOCHROME);
}
}

namespace nospark {
namespace ui {

LcdConsole &LcdConsole::init() {
  static LcdConsole console;
  return console;
}

LcdConsole::LcdConsole()
    : in_settings(false),
      sleeping(false),
      last_event(Timer::millis()),
      lcdState(new LcdStateBootup(lcd)),
      lcd(lcd_int) {
  updateBacklightType(lcd_int);
}

void LcdConsole::setState(LcdState *newState) {
  if (lcdState != 0) delete lcdState;

  lcd.clear();
  lcdState = newState;
  lcdState->draw();
}

void LcdConsole::update() {
  const bool keepState = lcdState->draw();
  if (!keepState && in_settings) {
    in_settings = false;
    if (State::get().fault == State::NOTHING_WRONG)
      setState(new LcdStateRunning(lcd));
    else
      setState(new LcdStateError(lcd));
  }
}

void LcdConsole::onEvent(const event::Event &event) {
  switch (event.id) {
    case EVENT_UPDATE:
      update();
      break;

    case EVENT_KEYHOLD:
      if (!sleeping) {
        if (!in_settings) {
          in_settings = true;
          setState(new LcdStateSettings(lcd));
        } else {
          lcdState->advance();
        }
      }
      break;

    case EVENT_KEYUP:
      if (!sleeping) lcdState->select();
      break;

    case EVENT_CONTROLLER_STATE:
      if (event.param == State::RUNNING)
        setState(new LcdStateRunning(lcd));
      else if (event.param == State::FAULT)
        setState(new LcdStateError(lcd));
      break;

    case EVENT_REQUEST_SLEEP:
      sleeping = event.param;
      in_settings = false;
      last_event = Timer::millis();
      if (sleeping)
        setState(new LcdStateSleeping(lcd));
      else
        setState(new LcdStateRunning(lcd));
      break;

    case EVENT_SETTINGS_CHANGED:
      updateBacklightType(lcd_int);
      break;
  }

  updateSleepState(event);
}

void LcdConsole::updateSleepState(const event::Event &event) {
  if (event.id == EVENT_UPDATE && State::get().j1772 == J1772Pilot::STATE_A) {
    if (!sleeping && (Timer::millis() - last_event) > SLEEP_DELAY_MS) {
      Settings settings;
      EepromSettings::load(settings);
      if (settings.sleep_mode != 2)  // 2 = No sleepmode...
      {
        sleeping = 1;
        setState(new LcdStateSleeping(lcd));
      } else {
        last_event = Timer::millis();
      }
    }

  } else if (isWakeEvent(event.id)) {
    last_event = Timer::millis();
    if (sleeping) {
      setState(new LcdStateRunning(lcd));
      in_settings = false;
      sleeping = 0;
    }
  }
}
}
}
