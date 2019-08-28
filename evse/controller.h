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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "board/gfci.h"
#include "board/j1772pilot.h"
#include "board/pin.h"
#include "board/relays.h"
#include "board/sleepindicator.h"
#include "event/handler.h"
#include "evse/state.h"
#include "utils/cpp.h"

namespace nospark {
namespace evse {

class Controller : public event::Handler {
  Controller();
  ~Controller();

 public:
  static Controller &init();

 private:
  void onEvent(const event::Event &event) override;

  void update();

  // The state is only updated when the J1772 state
  // changes. |force_update| can be used to force
  // a state updated regardless of the J1772 state.
  void updateRunning(bool force_update = false);

  void enableCharge(const bool enable);
  void updateChargeCurrent(const bool enablePwm = false);

  void setFault(const State::ControllerFault fault);

  board::Relays relays;
  board::GFCI gfci;
  board::J1772Pilot j1772;
  board::SleepIndicator sleep_status;

  DISALLOW_COPY_AND_ASSIGN(Controller);
};

}  // namespace evse
}  // namespace nospark
