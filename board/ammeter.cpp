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

#include "ammeter.h"
#include "pins.h"

namespace
{
    inline uint16_t normalize(const uint16_t adc)
    {
        if (adc > 512)
            return adc - 512;
        return 512 - adc;
    }

    inline uint32_t square(const uint16_t val)
    {
        return static_cast<uint32_t>(val) * val;
    }

    uint16_t square_root(uint32_t val)
    {
        if (val < 2)
            return val;

        uint16_t min = 1;
        uint16_t max = 8192; // <-- This is optimized; not a univeral function!

        uint16_t test = 0;
        while (1)
        {
            test = (max - min) / 2 + min;

            const uint32_t sq_t1 = square(test);
            const uint32_t sq_t2 = square(test+1);

            if ((sq_t1 > val) != (sq_t2 > val))
                break;

            if (sq_t1 > val)
                max = test;
            else
                min = test;
        }

        return test;
    }
}

namespace board
{

Ammeter& Ammeter::get()
{
    static Ammeter meter;
    return meter;
}

uint32_t Ammeter::sample()
{
   return get().sample_impl();
}

Ammeter::Ammeter()
    : pin(PIN_AMMETER)
{
}

uint32_t Ammeter::sample_impl()
{
    uint32_t sum = 0;
    uint16_t last_sample = 0;
    uint16_t sample = pin.analogRead();
    uint8_t samples = 1;

    uint8_t zero_crossings = 0;
    while (zero_crossings < 3 && samples != 0)
    {
        last_sample = sample;
        sample = pin.analogRead();
        ++samples;
    
        if ( (sample >= 512 && last_sample < 512)
          || (sample < 512 && last_sample >= 512))
        {
            if (zero_crossings == 0 || samples > 25)
                ++zero_crossings;

            if (zero_crossings == 1)
            {
                samples = 1;
                sum = square(normalize(sample));
                continue;
            }
        }
        
        sum += square(normalize(sample));
    }
    
    // During debugging, this calculated Vrms correctly:
    // const uint32_t mv_rms = square_root(sum) 25 / 71;

    // The aquare root of the sum of all samples of a 5V
    // sine wave comes out to around 5020. Mapping a 100A
    // range onto this value, we arrive at this formula:
    //         amps = sqr(sum) / 5020 * 100
    // To make this more integer math friendly, we can
    // rearrange and reduct this to (and convert to 100mA's):
    return square_root(sum) * 50l / 251l;
}

}
