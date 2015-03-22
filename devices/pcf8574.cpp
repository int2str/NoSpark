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

#include "pcf8574.h"

namespace devices
{

PCF8574::PCF8574(const uint8_t i2c_addr)
    : i2c_addr(i2c_addr)
    , i2c(I2CMaster::get())
    , ddr(0xFF)
{
}

void PCF8574::ioDir(const uint8_t io)
{
    ddr = io;
}

uint8_t PCF8574::read()
{
    uint8_t b = 0;
    i2c.read(i2c_addr, &b, 1);
    return b & ddr;
}

void PCF8574::write(const uint8_t b)
{
    uint8_t b2 = b & ~ddr;
    i2c.write(i2c_addr, &b2, 1);
}

}
