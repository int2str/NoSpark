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

#include "stream/rapistream.h"
#include "ui/serialapi.h"
#include "utils/cpp.h"

namespace nospark {
namespace ui {

// Simple command line interface for control and monitoring.
class SerialApi {
 public:
  explicit SerialApi(stream::UartStream &uart);
  bool handleCommand(const char *buffer, const uint8_t len);

 private:
  stream::RapiStream rapi;

  void handleGet(const char *buffer);
  void handleSet(const char *buffer);
  void handleFunction(const char *buffer);

  DISALLOW_COPY_AND_ASSIGN(SerialApi);
};

}  // namespace ui
}  // namespace nospark
