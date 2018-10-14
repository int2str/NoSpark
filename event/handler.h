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

#include "event/event.h"
#include "utils/cpp.h"

namespace nospark {
namespace event {

// Abstract base clas for an event receiver. Derived classes will
// be automatically registered with the event looper.
class Handler {
public:
  Handler();
  virtual ~Handler();

  virtual void onEvent(const Event &event) = 0;

  DISALLOW_COPY_AND_ASSIGN(Handler);
};
}
}
