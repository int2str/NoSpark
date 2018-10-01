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

#include "board/ammeter.h"
#include "event/loop.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "system/timer.h"
#include "chargemonitor.h"
#include "events.h"

#define VOLTAGE 240

using nospark::event::Event;
using nospark::event::Loop;
using nospark::evse::Settings;
using nospark::evse::EepromSettings;
using nospark::evse::State;

namespace
{
    void saveChargeStats(const uint32_t kwh)
    {
        Settings settings;
        EepromSettings::load(settings);

        settings.kwh_total += kwh;
        settings.kwh_year += kwh;
        settings.kwh_month += kwh;
        settings.kwh_week += kwh;

        EepromSettings::save(settings);
    }

    uint32_t getChargeLimit()
    {
        Settings settings;
        EepromSettings::load(settings);
        return settings.kwh_limit * 1000l;
    }

    void setKwhLimited(const bool limited)
    {
        State &state = State::get();
        if (!limited && state.ready == State::KWH_LIMIT)
        {
            state.ready = State::READY;
            Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));

        } else if (limited && state.ready == State::READY) {
            state.ready = State::KWH_LIMIT;
            Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));
        }
    }
}

namespace nospark
{
namespace evse
{

ChargeMonitor& ChargeMonitor::get()
{
    static ChargeMonitor monitor;
    return monitor;
}

ChargeMonitor::ChargeMonitor()
    : time_start_ms(0)
    , time_stop_ms(0)
    , last_sample(0)
    , watt_seconds(0)
{
}

bool ChargeMonitor::isCharging() const
{
    return (time_start_ms > 0 && time_stop_ms == 0);
}

uint32_t ChargeMonitor::chargeDuration() const
{
    if (time_start_ms == 0)
        return 0;
    if (time_stop_ms != 0)
        return time_stop_ms - time_start_ms;
    return system::Timer::millis() - time_start_ms;
}

uint32_t ChargeMonitor::chargeCurrent() const
{
    return isCharging() ? current_samples.get() : 0;
}

uint32_t ChargeMonitor::wattHours() const
{
    return watt_seconds / 3600;
}

void ChargeMonitor::update()
{
    if (isCharging())
    {
        current_samples.push(ammeter.sample());
        const uint32_t now = system::Timer::millis();
        if ((now - last_sample) > 1000)
        {
            // Update charge energy
            watt_seconds += current_samples.get() * VOLTAGE / 1000;
            last_sample = now;

            // Check charge limit
            const uint32_t charge_limit = getChargeLimit();
            if (charge_limit)
            {
                State &state = State::get();
                if (state.ready == State::READY && wattHours() > charge_limit)
                    setKwhLimited(true);
            }
        }
    }
}

void ChargeMonitor::chargeStateChanged(const bool charging)
{
    if (charging)
    {
        // Record charge start time
        time_start_ms = system::Timer::millis();
        time_stop_ms = 0;
        watt_seconds = 0;
    } else {
        // Record end time
        if (time_start_ms != 0 && time_stop_ms == 0)
        {
            time_stop_ms = system::Timer::millis();
            saveChargeStats(wattHours() / 1000);
        }
    }

    current_samples.clear();
}

void ChargeMonitor::onEvent(const event::Event& event)
{
    switch (event.id)
    {
        case EVENT_UPDATE:
            update();
            break;

        case EVENT_CHARGE_STATE:
            chargeStateChanged(event.param);
            break;
    }
}

}
}
