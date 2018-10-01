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
#include "evse/state.h"
#include "utils/math.h"
#include "events.h"
#include "temperaturemonitor.h"

#define TEMPERATURE_ALARM_LEVEL1    60
#define TEMPERATURE_ALARM_LEVEL2    70
#define TEMPERATURE_ALARM_LEVEL3    75

#define MINIMUM_CHARGE_CURRENT       6

namespace
{
    using nospark::devices::DS3231;
    using nospark::evse::State;
    using nospark::evse::TemperatureMonitor;

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

    void updateCurrentLimit(const TemperatureMonitor::TemperatureState &temp_state)
    {
        uint8_t amps = State::get().max_amps_target;

        switch (temp_state)
        {
            case TemperatureMonitor::ELEVATED:
                amps = nospark::utils::max(amps/2, MINIMUM_CHARGE_CURRENT);
                break;

            case TemperatureMonitor::HIGH:
                amps = MINIMUM_CHARGE_CURRENT;
                break;

            case TemperatureMonitor::CRITICAL:
                amps = 0;
                break;

            case TemperatureMonitor::NOMINAL:
                break;
        }

        State::get().max_amps_limit = amps;
    }
}

namespace nospark
{
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
    switch (event.id)
    {
        case EVENT_UPDATE:
            update();
            break;

        case EVENT_MAX_AMPS_CHANGED:
            update(true);
            break;
    }
}

void TemperatureMonitor::update(bool force_update)
{
    const uint8_t temp = readTemp();
    if (force_update || (temp != last_temp))
    {
        const TemperatureState new_state = temp2State(temp);
        const TemperatureState old_state = temp2State(last_temp);

        // TODO: Debounce state changes
        if (force_update || (new_state != old_state))
        {
            updateCurrentLimit(new_state);
            event::Loop::post(event::Event(EVENT_TEMPERATURE_ALERT,
                new_state == TemperatureMonitor::CRITICAL ? 1 : 0));
        }

        last_temp = temp;
    }
}

}
}
