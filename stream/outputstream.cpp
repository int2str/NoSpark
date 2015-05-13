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
        write(str);
    }
    flags = 0;
    return *this;
}

OutputStream& OutputStream::operator<< (const uint8_t val)
{
    if (flags == 0)
    {
        char buffer[6] = {0};
        utoa(val, buffer, 10);
        return (*this << buffer);
    }

    if (flags & (1 << Flags::PAD_BCD))
    {
        *this << static_cast<char>('0' + (val >> 4));
        *this << static_cast<char>('0' + (val & 0x0F));

    } else if (flags & (1 << Flags::PAD_ZERO)) {
        if (val < 10)
            *this << static_cast<char>('0');
        else
            *this << static_cast<char>('0' + val / 10);
        *this << static_cast<char>('0' + val % 10);

    } else if (flags & (1 << Flags::PAD_SPACE)) {
        if (val < 10)
            *this << static_cast<char>(' ');
        else
            *this << static_cast<char>('0' + val / 10);
        *this << static_cast<char>('0' + val % 10);
    }

    flags = 0;
    return *this;
}

OutputStream& OutputStream::operator<< (const uint32_t val)
{
    char buffer[6] = {0};
    ltoa(val, buffer, 10);
    return (*this << buffer);
}

}
