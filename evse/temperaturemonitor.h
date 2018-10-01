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

#include <stdbool.h>
#include <stdint.h>

#include "event/handler.h"
#include "utils/cpp.h"

namespace nospark
{
namespace evse
{

// Reads a temperature source (or more down the road?)
// and fires over-temperature events (defined in events.h).
// TODO: Use dependency injection to provide temperatue source
class TemperatureMonitor : public event::Handler
{
    TemperatureMonitor();

public:
    enum TemperatureState
    {
        NOMINAL
      , ELEVATED
      , HIGH
      , CRITICAL
    };

    static TemperatureMonitor& init();
    static TemperatureState getState();

private:
    void onEvent(const event::Event &event) override;
    void update(bool force_update = false);

    uint8_t last_temp;

    DISALLOW_COPY_AND_ASSIGN(TemperatureMonitor);
};

}
}
