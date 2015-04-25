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

#include "utils/math.h"
#include "ammeter.h"
#include "pins.h"

// Apparently this is calculated from the burden
// resistor (22 Ohm in the OpenEVSE v3) and the
// number of ADC steps.
//     5V / 1024 steps = 4.882mV/step
//     22 Ohm / 4.882mV = 4.5056
//     1 / 4.5056 * 1000 (to get to mA) = 222
// Slightly tweaked for accuracy...
#define CURRENT_SCALE_FACTOR 220l

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

    // The uint8_t roll-over is really useful here since
    // 255 samples at 0.11ms is about 28ms. Thus we don't
    // need to use millis() or anything to make the timeout
    // happen.
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
            {
                if (zero_crossings == 0)
                    samples = 1;
                ++zero_crossings;
            }

        }

        sum += square(normalize(sample));
    }

    if (samples)
        sum /= samples;
    else
        sum = 0;

    return utils::square_root<uint32_t, 512>(sum) * CURRENT_SCALE_FACTOR;
}

}
