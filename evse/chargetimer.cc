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

#include "evse/chargetimer.h"
#include "devices/ds3231.h"
#include "event/loop.h"
#include "events.h"
#include "evse/settings.h"
#include "evse/state.h"

#include "system/timer.h"

#define CHARGE_TIMER_UPDATE_DELAY_MS 60000

using nospark::devices::DS3231;
using nospark::event::Event;
using nospark::event::Loop;
using nospark::evse::Settings;
using nospark::evse::EepromSettings;
using nospark::evse::State;

namespace {
void setChargeLimited(const bool limited) {
  State &state = State::get();
  if (!limited && state.ready == State::SCHEDULED) {
    state.ready = State::READY;
    Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));

  } else if (limited && state.ready == State::READY) {
    state.ready = State::SCHEDULED;
    Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));
  }
}
}

namespace nospark {
namespace evse {

ChargeTimer &ChargeTimer::init() {
  static ChargeTimer timer;
  return timer;
}

ChargeTimer::ChargeTimer() : can_charge(true) {}

ChargeTimer::~ChargeTimer() {}

void ChargeTimer::checkTime() {
  bool can_charge_now = true;

  Settings settings;
  EepromSettings::load(settings);

  if (settings.charge_timer_enabled) {
    DS3231 &rtc = DS3231::get();
    rtc.read();

    uint16_t now = (rtc.hour << 8) | rtc.minute;
    uint16_t start_time = settings.charge_timer_t1;
    uint16_t end_time = settings.charge_timer_t2;
    if (now < start_time) now += (24 << 8);
    if (end_time < start_time) end_time += (24 << 8);

    can_charge_now = (now >= start_time) && (now <= end_time);
  }

  if (can_charge != can_charge_now) {
    can_charge = can_charge_now;
    setChargeLimited(!can_charge);
  }

  event::Loop::postDelayed(Event(EVENT_UPDATE_CHARGE_TIMER),
                           CHARGE_TIMER_UPDATE_DELAY_MS);
}

void ChargeTimer::onEvent(const event::Event &event) {
  if ((event.id == EVENT_UPDATE_CHARGE_TIMER) ||
      (event.id == EVENT_SETTINGS_CHANGED) ||
      (event.id == EVENT_CONTROLLER_STATE && event.param == State::RUNNING))
    checkTime();
}
}
}
