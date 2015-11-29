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
#include "utils/bcd.h"

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
    bool checkPresent(devices::I2CMaster &i2c, const uint8_t i2c_addr)
    {
        uint8_t temp = DS3231_REG_STATUS;
        i2c.write(i2c_addr, &temp, 1);
        temp = 0;
        i2c.read(i2c_addr, &temp, 1);
        return temp != 0;
    }
}

namespace devices
{

void TM::setWeekday()
{
    static uint8_t wkd[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    uint8_t y = year - (month < 3);

    weekday = (y + y / 4 + wkd[month - 1] + day) % 7 + 1;
}

DS3231& DS3231::get()
{
    static DS3231 rtc;
    return rtc;
}

DS3231::DS3231()
    : i2c_addr(DS3231_I2C_ADDRESS)
    , i2c(I2CMaster::get())
    , present(checkPresent(i2c, i2c_addr))
{
}

DS3231::~DS3231()
{
}

bool DS3231::isPresent() const
{
    return present;
}

void DS3231::read(TM &t)
{
    uint8_t buffer[8] = {0};
    buffer[0] = DS3231_REG_SECOND;

    i2c.write(i2c_addr, buffer, 1);
    i2c.read(i2c_addr, buffer + 1, 7);

    t.second  = utils::bcd2dec(buffer[1]);
    t.minute  = utils::bcd2dec(buffer[2]);
    t.hour    = utils::bcd2dec(buffer[3]);
    t.weekday = utils::bcd2dec(buffer[4]);
    t.day     = utils::bcd2dec(buffer[5]);
    t.month   = utils::bcd2dec(buffer[6]);
    t.year    = utils::bcd2dec(buffer[7]);
}

void DS3231::write(TM &t)
{
    uint8_t buffer[8] = {
        DS3231_REG_SECOND
      , utils::dec2bcd(t.second)
      , utils::dec2bcd(t.minute)
      , utils::dec2bcd(t.hour)
      , utils::dec2bcd(t.weekday)
      , utils::dec2bcd(t.day)
      , utils::dec2bcd(t.month)
      , utils::dec2bcd(t.year)
    };
    i2c.write(i2c_addr, buffer, 8);
}

int8_t DS3231::readTemp()
{
    uint8_t buffer[2] = {DS3231_REG_TEMP_MSB, 0};
    i2c.write(i2c_addr, buffer, 1);
    buffer[0] = 0;
    i2c.read(i2c_addr, buffer, 2);
    return (int8_t)(buffer[0]);
}

}
