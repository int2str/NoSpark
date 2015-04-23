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
#include "system/timer.h"
#include "chargemonitor.h"
#include "events.h"

#define VOLTAGE 240

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
        current_samples.push(board::Ammeter::sample());
        const uint32_t now = system::Timer::millis();
        if ((now - last_sample) > 1000)
        {
            watt_seconds += current_samples.get() * VOLTAGE / 1000;
            last_sample = now;
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
        // Record end time only if we had started before
        // This avoids an unecessary millis() call for
        // initialization etc.
        if (time_start_ms != 0)
            time_stop_ms = system::Timer::millis();
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
