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

#include <stdlib.h>
#include <avr/pgmspace.h>

#include "stream/outputstream.h"

namespace stream
{

OutputStream::OutputStream()
    : flags(0)
{
}

OutputStream::~OutputStream()
{
}

OutputStream& OutputStream::operator<< (const Flags flag)
{
    flags |= (1 << flag);
    return *this;
}

OutputStream& OutputStream::operator<< (const Spaces spaces)
{
    for (size_t i = 0; i != spaces.num; ++i)
        *this << ' ';
    return *this;
}

OutputStream& OutputStream::operator<< (const char ch)
{
    write(ch);
    return *this;
}

OutputStream& OutputStream::operator<< (const char* str)
{
    if (flags & (1 << Flags::PGM))
    {
        while (pgm_read_byte(str))
            write(pgm_read_byte(str++));
    } else {
        while (*str)
            write(*str++);
    }
    flags = 0;
    return *this;
}

OutputStream& OutputStream::operator<< (const uint8_t val)
{
    char buffer[6] = {0};
    utoa(val, buffer, 10);

    if (val < 10)
    {
        if (flags & (1 << Flags::PAD_ZERO))
            *this << '0';
        else if (flags & (1 << Flags::PAD_SPACE))
            *this << ' ';
    }

    flags = 0;
    return *this << buffer;
}

OutputStream& OutputStream::operator<< (const int8_t val)
{
    char buffer[6] = {0};
    itoa(val, buffer, 10);

    if (flags & (1 << Flags::PAD_SPACE))
        if (val > -10 && val < 100)
        {
            *this << ' ';
            if (val > -1 && val < 10)
                *this << ' ';
        }

    flags = 0;
    return *this << buffer;
}

OutputStream& OutputStream::operator<< (const uint32_t val)
{
    char buffer[6] = {0};
    ltoa(val, buffer, 10);
    return *this << buffer;
}

}
