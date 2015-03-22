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

#include "mcp23017.h"

#define MCP23017_IODIRA     0x00
#define MCP23017_IODIRB     0x01
#define MCP23017_IPOLA      0x02
#define MCP23017_IPOLB      0x03
#define MCP23017_GPINTENA   0x04
#define MCP23017_GPINTENB   0x05
#define MCP23017_DEFVALA    0x06
#define MCP23017_DEFVALB    0x07
#define MCP23017_INTCONA    0x08
#define MCP23017_INTCONB    0x09
#define MCP23017_IOCONA     0x0A
#define MCP23017_IOCONB     0x0B
#define MCP23017_GPPUA      0x0C
#define MCP23017_GPPUB      0x0D
#define MCP23017_INTFA      0x0E
#define MCP23017_INTFB      0x0F
#define MCP23017_INTCAPA    0x10
#define MCP23017_INTCAPB    0x11
#define MCP23017_GPIOA      0x12
#define MCP23017_GPIOB      0x13
#define MCP23017_OLATA      0x14
#define MCP23017_OLATB      0x15

namespace devices
{

MCP23017::MCP23017(const uint8_t i2c_addr)
    : i2c_addr(i2c_addr)
    , i2c(I2CMaster::get())
{
}

void MCP23017::ioDir(const uint8_t a, const uint8_t b)
{
    uint8_t buffer[3] = {MCP23017_IODIRA, a, b};
    i2c.write(i2c_addr, buffer, 3);
}

void MCP23017::pullUp(const uint8_t a, const uint8_t b)
{
    uint8_t buffer[3] = {MCP23017_GPPUA, a, b};
    i2c.write(i2c_addr, buffer, 3);
}

uint16_t MCP23017::read()
{
    uint8_t buffer[2] = {MCP23017_GPIOA, 0};
    i2c.write(i2c_addr, buffer, 1);
    i2c.read(i2c_addr, buffer, 2);
    return (buffer[1] << 8) | buffer[0];
}

void MCP23017::write(const uint8_t a, const uint8_t b)
{
    uint8_t buffer[3] = {MCP23017_GPIOA, a, b};
    i2c.write(i2c_addr, buffer, 3);
}

uint8_t MCP23017::readA()
{
    uint8_t b = MCP23017_GPIOA;
    i2c.write(i2c_addr, &b, 1);
    i2c.read(i2c_addr, &b, 1);
    return b;
}

void MCP23017::writeA(const uint8_t b)
{
    uint8_t buffer[2] = {MCP23017_GPIOA, b};
    i2c.write(i2c_addr, buffer, 2);
}

uint8_t MCP23017::readB()
{
    uint8_t b = MCP23017_GPIOB;
    i2c.write(i2c_addr, &b, 1);
    i2c.read(i2c_addr, &b, 1);
    return b;
}

void MCP23017::writeB(const uint8_t b)
{
    uint8_t buffer[2] = {MCP23017_GPIOB, b};
    i2c.write(i2c_addr, buffer, 2);
}

}
