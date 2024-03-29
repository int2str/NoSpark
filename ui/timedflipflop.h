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

namespace nospark {
namespace ui {

// Toggle the state of a simple boolean at a given |duration|.
class TimedFlipFlop {
 public:
  explicit TimedFlipFlop(const uint16_t duration);
  bool operator()();

 private:
  const uint16_t duration;
  bool last_state;
  uint32_t last_change;
};

}  // namespace ui
}  // namespace nospark
