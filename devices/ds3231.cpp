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

#include "ds3231.h"

#define DS3231_I2C_ADDRESS      0x68

// Registers
#define DS3231_REG_SECOND       0x00    // 00-59
#define DS3231_REG_MINUTE       0x01    // 00-59
#define DS3231_REG_HOUR         0x02    // 00-23 / 01-12 + AM/PM (Bit 6 enables)
#define DS3231_REG_DAY_OF_WEEK  0x03    // 01-07
#define DS3231_REG_DAY          0x04    // 01-31
#define DS3231_REG_MONTH        0x05    // 01-12 + Bit 7 century
#define DS3231_REG_YEAR         0x06    // 00-99
// ... skipping alarm registers
#define DS3231_REG_CONTROL      0x0E
#define DS3231_REG_STATUS       0x0F
#define DS3231_REG_AGING        0x10
#define DS3231_REG_TEMP_MSB     0x11
#define DS3231_REG_TEMP_LSB     0x12

// Control register (0x0E)
#define DS3231_EOSC             7
#define DS3231_BBSQW            6
#define DS3231_CONV             5
#define DS3231_RS2              4
#define DS3231_RS1              3
#define DS3231_INTCN            2
#define DS3231_A2IE             1
#define DS3231_A1IE             0

// Status register
#define DS3231_OSF              7
#define DS3231_EN32KHZ          3
#define DS3231_BSY              2
#define DS3231_A2F              1
#define DS3231_A1F              0

namespace
{
    uint8_t bcd2dec(const uint8_t bcd)
    {
        return ((bcd >> 4) * 10) + (bcd & 0x0F);
    }

    uint8_t dec2bcd(const uint8_t dec)
    {
        return ((dec / 10) << 4) + (dec % 10);
    }
}

namespace devices
{

DS3231& DS3231::get()
{
    static DS3231 rtc;
    return rtc;
}

DS3231::DS3231()
    : i2c_addr(DS3231_I2C_ADDRESS)
    , i2c(board::I2CMaster::get())
{
}

DS3231::~DS3231()
{
}

void DS3231::readRaw(uint8_t *buffer, const uint8_t len)
{
    uint8_t reg_select[1] = {DS3231_REG_SECOND};
    i2c.write(i2c_addr, reg_select, 1);
    i2c.read(i2c_addr, buffer, len);
}

void DS3231::read()
{
    uint8_t buffer[7] = {0};
    readRaw(buffer, 7);

    second  = bcd2dec(buffer[0]);
    minute  = bcd2dec(buffer[1]);
    hour    = bcd2dec(buffer[2]);
    weekday = bcd2dec(buffer[3]);
    day     = bcd2dec(buffer[4]);
    month   = bcd2dec(buffer[5]);
    year    = bcd2dec(buffer[6]);
}

void DS3231::writeRaw(uint8_t *buffer, const uint8_t len)
{
    i2c.write(i2c_addr, buffer, len);
}

void DS3231::write()
{
    uint8_t buffer[8] = {
        DS3231_REG_SECOND
      , dec2bcd(second)
      , dec2bcd(minute)
      , dec2bcd(hour)
      , dec2bcd(weekday)
      , dec2bcd(day)
      , dec2bcd(month)
      , dec2bcd(year)
    };
    writeRaw(buffer, 8);
}

uint8_t DS3231::readTemp()
{
    uint8_t buffer[2] = {DS3231_REG_TEMP_MSB, 0};
    i2c.write(i2c_addr, buffer, 1);
    i2c.read(i2c_addr, buffer, 2);
    return buffer[0];
}

}
