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

#include "i2c_master.h"
#include "utils/cpp.h"

namespace nospark {
namespace devices {

class MCP23008 {
 public:
  explicit MCP23008(const uint8_t i2c_addr);

  // 1 = input, 0 = output
  // Note that this is exactly opposite of the AVR
  // DDR register bits.
  void ioDir(const uint8_t io);
  void pullUp(const uint8_t pu);

  uint8_t read();
  void write(const uint8_t b);

 private:
  const uint8_t i2c_addr;

  I2CMaster &i2c;

  DISALLOW_COPY_AND_ASSIGN(MCP23008);
};

}  // namespace devices
}  // namespace nospark
