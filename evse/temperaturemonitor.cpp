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

#include "devices/ds3231.h"
#include "event/loop.h"
#include "events.h"
#include "temperaturemonitor.h"

#define TEMPERATURE_ALARM_LEVEL1    50
#define TEMPERATURE_ALARM_LEVEL2    55
#define TEMPERATURE_ALARM_LEVEL3    60

using event::Event;
using event::Loop;

namespace
{
    using devices::DS3231;
    using evse::TemperatureMonitor;

    uint8_t readTemp()
    {
        DS3231 &rtc = DS3231::get();
        return rtc.readTemp();
    }

    TemperatureMonitor::TemperatureState temp2State(const uint8_t temp)
    {
        if (temp >= TEMPERATURE_ALARM_LEVEL3)
            return TemperatureMonitor::CRITICAL;

        else if (temp >= TEMPERATURE_ALARM_LEVEL2)
            return TemperatureMonitor::HIGH;

        else if (temp >= TEMPERATURE_ALARM_LEVEL1)
            return TemperatureMonitor::ELEVATED;

        return TemperatureMonitor::NOMINAL;
    }
}

namespace evse
{

TemperatureMonitor& TemperatureMonitor::init()
{
    static TemperatureMonitor temp;
    return temp;
}

TemperatureMonitor::TemperatureState TemperatureMonitor::getState()
{
    const TemperatureMonitor &tm = init();
    return temp2State(tm.last_temp);
}

TemperatureMonitor::TemperatureMonitor()
    : last_temp(0)
{
}

void TemperatureMonitor::onEvent(const event::Event& event)
{
    if (event.id == EVENT_UPDATE)
        update();
}

void TemperatureMonitor::update()
{
    const uint8_t temp = readTemp();

    if (temp != last_temp)
    {
        const TemperatureState new_state = temp2State(temp);
        const TemperatureState old_state = temp2State(last_temp);

        // TODO: Debounce state changes
        if (new_state != old_state)
            Loop::post(Event(EVENT_TEMPERATURE_ALERT, new_state));

        last_temp = temp;
    }
}

}
