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

#include "chargemonitor.h"
#include "board/ammeter.h"
#include "board/j1772pilot.h"
#include "event/loop.h"
#include "events.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "system/timer.h"

#define VOLTAGE 240

using nospark::board::J1772Pilot;
using nospark::event::Event;
using nospark::event::Loop;
using nospark::evse::Settings;
using nospark::evse::EepromSettings;
using nospark::evse::State;

namespace {

void saveChargeStats(const uint32_t kwh) {
  Settings settings;
  EepromSettings::load(settings);

  settings.kwh_total += kwh;
  settings.kwh_year += kwh;
  settings.kwh_month += kwh;
  settings.kwh_week += kwh;

  EepromSettings::save(settings);
}

uint32_t getChargeLimit() {
  Settings settings;
  EepromSettings::load(settings);
  return settings.kwh_limit * 1000l;
}

void setKwhLimited(const bool limited) {
  State &state = State::get();
  if (!limited && state.ready == State::KWH_LIMIT) {
    state.ready = State::READY;
    Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));

  } else if (limited && state.ready == State::READY) {
    state.ready = State::KWH_LIMIT;
    Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));
  }
}

}  // namespace

namespace nospark {
namespace evse {

ChargeMonitor &ChargeMonitor::get() {
  static ChargeMonitor monitor;
  return monitor;
}

ChargeMonitor::ChargeMonitor()
    : time_start_ms(0),
      last_sample(0),
      charge_time_ms(0),
      watt_seconds(0),
      reset_stats_on_charge(true) {}

bool ChargeMonitor::isCharging() const { return time_start_ms > 0; }

uint32_t ChargeMonitor::chargeDuration() const {
  return charge_time_ms + getActiveDuration();
}

uint32_t ChargeMonitor::chargeCurrent() const {
  return isCharging() ? current_samples.get() : 0;
}

void ChargeMonitor::beginSession() {
  time_start_ms = system::Timer::millis();
  current_samples.clear();

  if (reset_stats_on_charge) {
    watt_seconds = 0;
    charge_time_ms = 0;
    reset_stats_on_charge = false;
  }
}

void ChargeMonitor::endSession() {
  charge_time_ms += getActiveDuration();
  time_start_ms = 0;
  saveChargeStats(wattHours() / 1000);
}

uint32_t ChargeMonitor::getActiveDuration() const {
  if (time_start_ms == 0) return 0;

  uint32_t now = system::Timer::millis();
  if (now < time_start_ms) {
    // Timer overflow...
    // We're just going to split the difference for now. Theoretically, this
    // will cause a problem if we've been charging for >49 days. But for now,
    // we're just going to say this can't happen...
    return 0xFFFFFFFF - time_start_ms + now;
  }

  return now - time_start_ms;
}

uint32_t ChargeMonitor::wattHours() const { return watt_seconds / 3600; }

uint32_t ChargeMonitor::wattSeconds() const { return watt_seconds; }

void ChargeMonitor::update() {
  if (!isCharging()) return;

  current_samples.push(ammeter.sample());
  const uint32_t now = system::Timer::millis();
  if ((now - last_sample) < 1000) return;  // Too soon to update...

  // Update charge energy
  watt_seconds += current_samples.get() * VOLTAGE / 1000;
  last_sample = now;

  // Check charge limit
  const uint32_t charge_limit = getChargeLimit();
  if (charge_limit) {
    State &state = State::get();
    if (state.ready == State::READY && wattHours() > charge_limit)
      setKwhLimited(true);
  }
}

void ChargeMonitor::onEvent(const event::Event &event) {
  switch (event.id) {
    case EVENT_UPDATE:
      update();
      break;

    case EVENT_CHARGE_STATE:
      if (event.param)
        beginSession();
      else
        endSession();
      break;

    case EVENT_J1772_STATE:
      if (event.param == J1772Pilot::STATE_A) reset_stats_on_charge = true;
      break;
  }
}

}  // namespace evse
}  // namespace nospark
