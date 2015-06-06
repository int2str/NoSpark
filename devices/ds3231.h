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

#include <stdint.h>
#include <stdbool.h>

#include "utils/cpp.h"
#include "i2c_master.h"

namespace devices
{

class DS3231
{
    DS3231();
    ~DS3231();

public:
    static DS3231& get();

    bool isPresent() const;

    void read();
    void write();

    void readRaw(uint8_t *buffer, const uint8_t len);
    void writeRaw(uint8_t *buffer, const uint8_t len);

    // Returns temperature in deg. C (whole deg. only)
    uint8_t readTemp();

    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t weekday;

private:
    const uint8_t i2c_addr;
    I2CMaster& i2c;
    const bool present;

    DISALLOW_COPY_AND_ASSIGN(DS3231);
};

}
