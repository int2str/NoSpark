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

#include "evse/controller.h"
#include "board/pin.h"
#include "board/pins.h"
#include "event/loop.h"
#include "events.h"
#include "utils/math.h"

#define SUSPEND_RELAY_DELAY 3000

using nospark::board::GFCI;
using nospark::board::J1772Pilot;
using nospark::board::Pin;
using nospark::board::Relays;
using nospark::event::Event;
using nospark::event::Loop;

namespace {
using nospark::evse::State;

void setControllerState(const State::ControllerState state) {
  State::get().controller = state;
  Loop::post(Event(EVENT_CONTROLLER_STATE, state));
}

void setJ1772State(const J1772Pilot::State j1772) {
  State::get().j1772 = j1772;
  Loop::post(Event(EVENT_J1772_STATE, j1772));
}
}

namespace nospark {
namespace evse {

Controller &Controller::init() {
  static Controller controller;
  return controller;
}

Controller::Controller() { enableCharge(false); }

Controller::~Controller() {}

void Controller::update() {
  switch (State::get().controller) {
    case State::RUNNING:
      updateRunning();
      break;

    case State::BOOTUP:
      setControllerState(State::POST);
      break;

    case State::POST:
      if (!gfci.selfTest()) {
        setFault(State::FAULT_POST_GFCI);
      } else {
        j1772.setMode(J1772Pilot::HIGH);
        setControllerState(State::RUNNING);
      }
      break;

    case State::FAULT:
      break;
  }
}

void Controller::updateRunning(bool force_update) {
  State &state = State::get();

  // Check relay state for faults
  const Relays::RelayState relay_state = relays.checkStatus();
  switch (relay_state) {
    case Relays::OK:
    case Relays::UNKNOWN:
      break;

    case Relays::NO_GROUND:
      setFault(State::FAULT_RELAY_NO_GROUND);
      break;

    case Relays::STUCK:
      setFault(State::FAULT_RELAY_STUCK);
      break;
  }

  // Check J1772 state
  const auto j1772_state = j1772.getState();
  if (state.j1772 == j1772_state && !force_update)
    return;  // State hasn't changed...

  // J1772 state has changed...
  switch (j1772_state) {
    case J1772Pilot::STATE_E:  // <-- Error
      enableCharge(false);
      break;

    case J1772Pilot::STATE_A:  // <-- EV not connected
    case J1772Pilot::NOT_READY:
      if (state.ready == State::KWH_LIMIT) state.ready = State::READY;
      enableCharge(false);
      j1772.setMode(J1772Pilot::HIGH);
      break;

    case J1772Pilot::STATE_B:  // <-- EV Connected
    case J1772Pilot::STATE_D:  // <-- Vent required :(
    case J1772Pilot::DIODE_CHECK_FAILED:
      enableCharge(false);
      event::Loop::remove(Event(EVENT_SET_RELAY));
      if (state.ready == State::READY)
        updateChargeCurrent(true);
      else
        j1772.setMode(J1772Pilot::HIGH);
      break;

    case J1772Pilot::STATE_C:  // <-- Charging
      if (state.ready == State::READY) {
        updateChargeCurrent(true);
        enableCharge(true);
      } else {
        j1772.setMode(J1772Pilot::HIGH);
        event::Loop::postDelayed(Event(EVENT_SET_RELAY, 0),
                                 SUSPEND_RELAY_DELAY);
      }
      break;

    case J1772Pilot::UNKNOWN:
    case J1772Pilot::IMPLAUSIBLE:
      enableCharge(false);
      j1772.setMode(J1772Pilot::LOW);
      // TODO: Snap to error condition?
      break;
  }

  setJ1772State(j1772_state);
}

void Controller::onEvent(const event::Event &event) {
  switch (event.id) {
    case EVENT_UPDATE:
      update();
      break;

    case EVENT_SET_RELAY:
      enableCharge(event.param);
      break;

    case EVENT_GFCI_TRIPPED:
      setFault(State::FAULT_GFCI_TRIPPED);
      break;

    case EVENT_MAX_AMPS_CHANGED:
      updateChargeCurrent();
      break;

    case EVENT_READY_STATE_CHANGED:
      if (State::get().controller == State::RUNNING) {
        sleep_status.set(State::get().ready != State::READY);
        updateRunning(true);
      }
      break;

    case EVENT_TEMPERATURE_ALERT:
      if (event.param)
        setFault(State::FAULT_TEMPERATURE_CRITICAL);
      else
        updateChargeCurrent();
      break;
  }
}

void Controller::enableCharge(const bool enable) {
  if (enable) {
    if (gfci.selfTest()) {
      relays.set(true);
    } else {
      relays.set(false);
      setFault(State::FAULT_POST_GFCI);
    }
  } else {
    relays.set(false);
  }
}

void Controller::updateChargeCurrent(const bool enablePwm) {
  const uint8_t max_amps = State::get().max_amps_limit;

  // Can't charge right now :(
  if (max_amps == 0)
    j1772.setMode(J1772Pilot::HIGH);

  else if (enablePwm || j1772.getMode() == J1772Pilot::PWM)
    j1772.pwmAmps(max_amps);
}

void Controller::setFault(const State::ControllerFault fault) {
  enableCharge(false);
  j1772.setMode(J1772Pilot::LOW);
  State::get().fault = fault;
  setControllerState(State::FAULT);
}
}
}
