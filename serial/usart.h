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

#include "utils/cpp.h"

namespace nospark {
namespace serial {

// Non-interrupt, polling based implementation. Uses UART0.
// This is a singleton; there shall be only one.
class Usart {
 public:
  static Usart &get();

  uint8_t read();
  void write(const uint8_t b);

  bool avail() const;

 private:
  Usart();

  DISALLOW_COPY_AND_ASSIGN(Usart);
};

}  // namespace serial
}  // namespace nospark
