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

#include "mcp23008.h"

#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

namespace nospark::devices {

MCP23008::MCP23008(const uint8_t i2c_addr)
    : i2c_addr(i2c_addr), i2c(I2CMaster::get()) {}

void MCP23008::ioDir(const uint8_t io) {
  uint8_t buffer[2] = {MCP23008_IODIR, io};
  i2c.write(i2c_addr, buffer, 2);
}

void MCP23008::pullUp(const uint8_t pu) {
  uint8_t buffer[2] = {MCP23008_GPPU, pu};
  i2c.write(i2c_addr, buffer, 2);
}

uint8_t MCP23008::read() {
  uint8_t b = MCP23008_GPIO;
  i2c.write(i2c_addr, &b, 1);
  i2c.read(i2c_addr, &b, 1);
  return b;
}

void MCP23008::write(const uint8_t b) {
  uint8_t buffer[2] = {MCP23008_GPIO, b};
  i2c.write(i2c_addr, buffer, 2);
}

}  // namespace nospark
