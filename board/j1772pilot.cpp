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

#include "board/adc.h"
#include "board/j1772pilot.h"
#include "board/pins.h"
#include "utils/atomic.h"
#include "utils/math.h"
#include "utils/pair.h"

#define J1772_SAMPLES 100

#define J1772_DIODE_THRESHOLD     256 // 1024 ADC_STEPS / 24V = ~43 STEPS/V
#define J1772_THRESHOLD_OFFSET    512 // Assuming this is half range (+12 <-> -12)
#define J1772_THRESHOLD_STEP      100 // 1023 / 10 ~= 100 :D

namespace
{
    using board::J1772Pilot;

    const J1772Pilot::State ADC_TO_STATE_MAP[] = {
        J1772Pilot::STATE_E,
        J1772Pilot::STATE_D,
        J1772Pilot::STATE_C,
        J1772Pilot::STATE_B,
        J1772Pilot::STATE_A,
        J1772Pilot::STATE_A // Rounding up buffer
    };

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
        OCR1B = 249 * duty_cycle / 100;
    }

    void pwmDisable()
    {
        utils::Atomic _atomic;
        TCCR1A = 0;
    }

    J1772Pilot::State stateFromSample(const uint16_t sample)
    {
        if (sample < J1772_THRESHOLD_OFFSET)
            return J1772Pilot::IMPLAUSIBLE;

        return ADC_TO_STATE_MAP[(sample - J1772_THRESHOLD_OFFSET) / J1772_THRESHOLD_STEP];
    }
}

namespace board
{

J1772Pilot::J1772Pilot()
    : last_state(NOT_READY)
    , mode(LOW)
{
}

void J1772Pilot::setMode(const Mode mode)
{
    if (mode == PWM)
        return;
    this->mode = mode;
    pwmDisable();
    setPinActive(mode == HIGH);
}

J1772Pilot::Mode J1772Pilot::getMode() const
{
    return mode;
}

void J1772Pilot::pwmAmps(const uint8_t amps)
{
    mode = PWM;
    pwmEnable(amp2duty(amps));
}

J1772Pilot::State J1772Pilot::getState(const bool force_update)
{
    if (J1772Pilot::getMode() == J1772Pilot::LOW)
    {
        last_state = NOT_READY;

    } else {
        Adc &adc = Adc::get();

        // Wait for fresh reading if necessary...
        if (force_update || last_state == NOT_READY)
            while(!adc.j1772Ready()) {}

        if (adc.j1772Ready())
        {
            const auto sample = adc.readJ1772();
            if (J1772Pilot::getMode() == J1772Pilot::PWM && sample.first > J1772_DIODE_THRESHOLD)
                last_state = DIODE_CHECK_FAILED;
            else
                last_state = stateFromSample(sample.second);
        }
    }

    return last_state;
}

}
