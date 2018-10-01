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

#include "bcd.h"

namespace nospark
{
namespace utils
{

uint8_t dec2bcd(uint8_t dec)
{
    uint8_t bcd = 0;
    while (dec >= 10)
    {
        bcd += 0x10;
        dec -= 10;
    }
    return bcd | dec;
}

uint8_t bcd2dec(const uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

}
}
