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

#include <avr/io.h>
#include "pin.h"

#define _AVR_REG(addr) (*reinterpret_cast<volatile uint8_t *>(addr))

#define _PIN    _AVR_REG(reg_pin)
#define _DDR    _AVR_REG(reg_pin + 1)
#define _PORT   _AVR_REG(reg_pin + 2)

namespace
{
    uint8_t bit2num(uint8_t b)
    {
        uint8_t n = 0;
        while (b != 1)
        {
            ++n;
            b >>= 1;
        }
        return n;
    }
}

namespace board
{

Pin::Pin(volatile uint8_t* reg_pin, const uint8_t b)
    : reg_pin(reg_pin)
    , b(b)
{
}

void Pin::io(const PinDirection d)
{
    switch (d)
    {
        case IN:
            _DDR &= ~b;
            break;

        case IN_PULLUP:
            _DDR &= ~b;
            _PORT |= b;
            break;

        case OUT:
            _DDR |= b;
            break;
    }
}

uint16_t Pin::analogRead()
{
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (1 << REFS0) | bit2num(b);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1<<ADSC)) {};
    return ADCW;
}

Pin& Pin::operator= (const uint8_t rhs)
{
    if (rhs)
        _PORT |= b;
    else
        _PORT &= ~b;

    return *this;
}

bool Pin::operator! () const
{
    return (_PIN & b) ? false : true;
}

}
