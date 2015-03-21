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
#include <avr/pgmspace.h>
#include <stdint.h>

#include "usart.h"

// The USART prescaler is not accurate at 115200 @ 16Mhz.
// Switched to 2x mode (U2X) to keep the error down. This
// changes the divisor to 8....
#define USART_BAUD_PRESCALE (((F_CPU  / USART_BAUD_RATE) / 8) - 1)

#define CR '\r'

namespace serial
{

Usart& Usart::get()
{
    static Usart usart;
    return usart;
}

Usart::Usart()
{
    UBRR0H = USART_BAUD_PRESCALE >> 8;
    UBRR0L = USART_BAUD_PRESCALE & 0xFF;

    UCSR0A |= (1 << U2X0);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void Usart::write(const uint8_t b)
{
    while (0 == (UCSR0A & (1 << UDRE0))) {}
    UDR0 = b;
}

void Usart::write(const char *str)
{
    while (*str)
        write(*str++);
}

void Usart::writeln(const char *str)
{
    write(str);
    write(CR);
}

void Usart::write_P(const char *str)
{
    while (pgm_read_byte(str))
        write(pgm_read_byte(str++));
}

void Usart::writeln_P(const char *str)
{
    write_P(str);
    write(CR);
}

bool Usart::avail() const
{
    return (0 != (UCSR0A & (1 << RXC0)));
}

uint8_t Usart::read()
{
    return UDR0;
}

}

