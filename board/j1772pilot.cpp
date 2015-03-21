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

#include "utils/atomic.h"
#include "j1772pilot.h"

namespace
{
    uint8_t amp2duty(const uint8_t amp)
    {
        uint16_t d = amp;
        if (amp <= 51)
            d = d * 10 / 6;
        else
            d = d * 2 / 5 + 64;
        return d;
    }

    void setPinActive(const bool active)
    {
        DDRB |= (1 << PB2);
        if (active)
            PORTB |= (1 << PB2);
        else
            PORTB &= ~(1 << PB2);
    }

    void pwmEnable(const uint8_t duty_cycle)
    {
        utils::Atomic _atomic;

        DDRB |= (1 << PB2);

        TCCR1A = (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
        TCCR1B = (1 << WGM13)  | (1 << WGM12) | (1 << CS11) | (1 << CS10);

        OCR1A = 249;
        OCR1B = (249 * duty_cycle / 100);
    }

    void pwmDisable()
    {
        utils::Atomic _atomic;
        TCCR1A = 0;
    }
}

namespace board
{

J1772Pilot& J1772Pilot::get()
{
    static J1772Pilot pilot;
    return pilot;
}

J1772Pilot::J1772Pilot()
    : mode(LOW)
{
}

void J1772Pilot::set(const J1772Mode mode)
{
    if (mode == PWM)
        return;
    get().mode = mode;
    pwmDisable();
    setPinActive(mode == HIGH);
}

void J1772Pilot::pwmAmps(const uint8_t amps)
{
    get().mode = PWM;
    pwmEnable(amp2duty(amps));
}

J1772Pilot::J1772Mode J1772Pilot::getMode()
{
    return get().mode;
}

}
