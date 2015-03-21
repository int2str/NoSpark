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
#include "j1772pilot.h"
#include "j1772status.h"
#include "pins.h"

// ADC sampling takes 0.11ms. So 100 samples = 11ms.
// That means ~10 cycles of the 1kHz carrier wave.
#define J1772_SAMPLES 100

#define J1772_DIODE_THRESHOLD     256 // 1024 ADC_STEPS / 24V = ~43 STEPS/V
#define J1772_THRESHOLD_OFFSET    512 // Assuming this is half range (+12 <-> -12)
#define J1772_THRESHOLD_STEP      100 // 1023 / 10 ~= 100 :D

namespace
{
    using board::J1772Status;
    
    J1772Status::J1772State stateFromSample(const uint16_t sample)
    {
        if (sample < J1772_THRESHOLD_OFFSET)
            return J1772Status::IMPLAUSIBLE;

        const J1772Status::J1772State states[] = {
            J1772Status::STATE_E,
            J1772Status::STATE_D,
            J1772Status::STATE_C,
            J1772Status::STATE_B,
            J1772Status::STATE_A,
            J1772Status::STATE_A // Rounding up buffer
        };

        return states[(sample - J1772_THRESHOLD_OFFSET) / J1772_THRESHOLD_STEP];
    }

    utils::Pair<uint16_t, uint16_t> samplePin(board::Pin& pin)
    {
        utils::Pair<uint16_t,uint16_t> reading = {1023, 0};

        for (uint8_t i = 0; i != J1772_SAMPLES; ++i)
        {
            const uint16_t value = pin.analogRead();
            reading.first = utils::min(reading.first, value);
            reading.second = utils::max(reading.second, value);
        }

        return reading;
    }
}

namespace board
{

J1772Status& J1772Status::get()
{
    static J1772Status status;
    return status;
}

J1772Status::J1772Status()
    : pin(PIN_J1772_STATUS)
{
    pin.io(Pin::IN);
}

J1772Status::J1772State J1772Status::read()
{
    if (J1772Pilot::getMode() == J1772Pilot::LOW)
        return NOT_READY;

    const auto sample = samplePin(pin);
    if (J1772Pilot::getMode() == J1772Pilot::PWM && sample.first > J1772_DIODE_THRESHOLD)
        return DIODE_CHECK_FAILED;

    return stateFromSample(sample.second);
}

}
