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

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "serial/usart.h"
#include "utils/atomic.h"

#define USART_BAUD_RATE     57600

#define USART_BUFFER_SIZE   64

// The USART prescaler is not accurate at 115200 @ 16Mhz.
// Switched to 2x mode (U2X) to keep the error down. This
// changes the divisor to 8....
#define USART_BAUD_PRESCALE (((F_CPU  / USART_BAUD_RATE) / 8) - 1)

namespace
{
    volatile uint8_t usart_buffer[USART_BUFFER_SIZE] = {0};
    volatile uint8_t usart_index_insert = 0;
    volatile uint8_t usart_index_read = 0;

    void usart_insert(const uint8_t b)
    {
        usart_buffer[usart_index_insert++] = b;

        if (usart_index_insert == USART_BUFFER_SIZE)
            usart_index_insert = 0;

        if (usart_index_insert  == usart_index_read)
            ++usart_index_read;
    }

    uint8_t usart_read()
    {
        utils::Atomic _atomic;
        if (usart_index_read >= USART_BUFFER_SIZE)
            usart_index_read = 0;

        if (usart_index_read == usart_index_insert)
            return -1;

        return usart_buffer[usart_index_read++];
    }

    uint8_t usart_available()
    {
        utils::Atomic _atomic;
        return usart_index_insert - usart_index_read;
    }

    ISR(USART_RX_vect)
    {
        usart_insert(UDR0);
    }
}

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
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void Usart::write(const uint8_t b)
{
    while (0 == (UCSR0A & (1 << UDRE0))) {}
    UDR0 = b;
}

uint8_t Usart::read()
{
    return usart_read();
}

bool Usart::avail() const
{
    return usart_available();
}

}

