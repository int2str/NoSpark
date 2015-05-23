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

#pragma once

#include <stdint.h>

#include "utils/pair.h"
#include "utils/cpp.h"

extern "C" void ADC_vect(void) __attribute__((signal));

namespace board
{

class Adc
{
    Adc();

public:
    static Adc& get();

    bool j1772Ready() const;
    utils::Pair<uint16_t, uint16_t> readJ1772();

    bool ammeterReady() const;
    uint32_t readAmmeter();

private:
    void update(const uint16_t val);

    volatile utils::Pair<uint16_t, uint16_t> j1772;
    volatile uint8_t j1772_samples;

    volatile uint32_t ammeter;
    volatile uint8_t ammeter_samples;
    volatile uint16_t ammeter_last_sample;
    volatile uint8_t ammeter_zero_crossings;

    volatile uint8_t mode;

    friend void ::ADC_vect(void);
    DISALLOW_COPY_AND_ASSIGN(Adc);
};

}
