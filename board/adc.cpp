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

#include "board/adc.h"
#include "board/pins.h"
#include "utils/atomic.h"
#include "utils/math.h"

// ADC mode; what's currently being sampled
#define ADC_AMMETER    0
#define ADC_J1772      1

// 60 samples at slow ADC speeds covers about 5-6 iterations of the 1kHz wave
#define J1772_SAMPLES  60

// ISR lives in global namespace to be "friend-able" by the Adc class...
ISR(ADC_vect)
{
    board::Adc::get().update(ADCW);
}

namespace
{
    uint16_t normalize(const uint16_t adc)
    {
        if (adc > 512)
            return adc - 512;
        return 512 - adc;
    }

    uint32_t square(const uint16_t val)
    {
        return static_cast<uint32_t>(val) * val;
    }

    // Code by Wilco Dijkstra, found here:
    // http://www.finesse.demon.co.uk/steven/sqrt.html
    //
    // Note that this function is actually SLOWER than
    // utils::square_root(), even the new, un-optimized
    // version of that function. However, this function
    // compiles to much less code. Largely because of the
    // lack of large unsigned multiplication.
    uint32_t square_root(uint32_t val)
    {
        uint32_t root = 0;
        uint32_t bit = (val >= 0x10000) ? (1l << 30) : (1l << 14);

        do
        {
            const uint32_t trial = root + bit;
            if (val >= trial)
            {
                val -= trial;
                root = trial + bit;
            }
            root >>= 1;
            bit >>= 2;
        } while (bit);

        return root;
    }
}

namespace board
{

Adc& Adc::get()
{
    static Adc adc;
    return adc;
}

Adc::Adc()
    : j1772(1023, 0)
    , j1772_samples(0)
    , ammeter(0)
    , ammeter_samples(0)
    , ammeter_last_sample(0)
    , ammeter_zero_crossings(0)
    , mode(0)
{
    // Set up ADC; slow speed (clk/128), interrupt enabled
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Internal reference
    ADMUX = (1 << REFS0) | mode;

    // Start first conversion; ISR will re-start conversion
    ADCSRA |= (1 << ADSC);
}

void Adc::update(const uint16_t val)
{
    if (mode == ADC_AMMETER && ammeter_zero_crossings < 3 && ammeter_samples < 0xFF)
    {
        if (ammeter_samples++)
        {
            if ( (val >= 512 && ammeter_last_sample < 512)
              || (val < 512 && ammeter_last_sample >= 512))
            {
                if (ammeter_zero_crossings == 0 || ammeter_samples > 25)
                {
                    if (ammeter_zero_crossings == 0)
                        ammeter_samples = 1;
                    ++ammeter_zero_crossings;
                }
            }

            ammeter += square(normalize(val));
        }
        ammeter_last_sample = val;

    } else if (mode == ADC_J1772 && j1772_samples < J1772_SAMPLES) {
            j1772.first = utils::min(j1772.first, val);
            j1772.second = utils::max(j1772.second, val);
            ++j1772_samples;

    } else {
        if (++mode > ADC_J1772)
            mode = 0;
        ADMUX = (1 << REFS0) | mode;
    }

    ADCSRA |= (1 << ADSC);
}

bool Adc::j1772Ready() const
{
    return j1772_samples == J1772_SAMPLES;
}

utils::Pair<uint16_t, uint16_t> Adc::readJ1772()
{
    utils::Atomic _atomic;

    utils::Pair<uint16_t, uint16_t> ret(0, 0);
    ret.first = j1772.first;
    ret.second = j1772.second;

    j1772.first = 1023;
    j1772.second = 0;
    j1772_samples = 0;

    return ret;
}

bool Adc::ammeterReady() const
{
    return (ammeter_zero_crossings == 3 || ammeter_samples == 0xFF);
}

uint32_t Adc::readAmmeter()
{
    utils::Atomic _atomic;

    if (ammeter_zero_crossings == 3 && ammeter_samples != 0)
        ammeter /= ammeter_samples;
    else
        ammeter = 0;
    uint32_t ret = square_root(ammeter);

    ammeter = 0;
    ammeter_zero_crossings = 0;
    ammeter_samples = 0;

    return ret;
}

}
