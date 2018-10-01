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

#include "board/ammeter.h"
#include "event/handler.h"
#include "utils/cpp.h"
#include "utils/movingaverage.h"

#define MOVING_AVERAGE_WINDOW   64

namespace nospark
{
namespace evse
{

class ChargeMonitor : public event::Handler
{
    ChargeMonitor();

public:
    static ChargeMonitor& get();

    bool isCharging() const;
    uint32_t chargeDuration() const;
    uint32_t chargeCurrent() const;
    uint32_t wattHours() const;

private:
    void update();
    void chargeStateChanged(const bool charging);

    void onEvent(const event::Event &event) override;

    board::Ammeter ammeter;
    uint32_t time_start_ms;
    uint32_t time_stop_ms;
    uint32_t last_sample;

    uint32_t watt_seconds;

    utils::MovingAverage<uint32_t, MOVING_AVERAGE_WINDOW> current_samples;

    DISALLOW_COPY_AND_ASSIGN(ChargeMonitor);
};

}
}
