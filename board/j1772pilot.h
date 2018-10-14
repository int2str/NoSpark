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

#include <stdint.h>

#include "board/pin.h"
#include "utils/cpp.h"

namespace nospark {
namespace board {

// Uses timer1 for PWM, thus PIN 10 is automatically used,
// no need to abstract it.
class J1772Pilot {
public:
  enum Mode { HIGH, LOW, PWM };

  enum State {
    UNKNOWN,
    STATE_A, // 12V; Not connected
    STATE_B, //  9V; Connected, ready
    STATE_C, //  6V; Charging
    STATE_D, //  3V; Charging, vent required
    STATE_E, //  0V; Error
    DIODE_CHECK_FAILED,
    NOT_READY,  // Pilot held low
    IMPLAUSIBLE // Reading doesn't make sense
  };

  J1772Pilot();

  // Set to HIGH or LOW only, use pwmAmps to enable PWM
  void setMode(const Mode mode);
  Mode getMode() const;
  void pwmAmps(const uint8_t amps);

  State getState(const bool force_update = false);

private:
  State last_state;
  Mode mode;

  DISALLOW_COPY_AND_ASSIGN(J1772Pilot);
};

} // namespace board
} // namespace nospark
