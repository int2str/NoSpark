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

#include "board/pin.h"
#include "event/handler.h"
#include "utils/cpp.h"

namespace nospark {
namespace board {

// Simple pulse generator used for debugging/benchmarking.
// Toggles SCK pin.
class Heartbeat : public event::Handler {
  Heartbeat();

 public:
  static Heartbeat& init();

 private:
  Pin pin_;
  void onEvent(const event::Event& event) override;

  DISALLOW_COPY_AND_ASSIGN(Heartbeat);
};

}  // namespace board
}  // namespace nospark
