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

#include "board/j1772pilot.h"
#include "event/loop.h"
#include "evse/temperaturemonitor.h"
#include "system/timer.h"
#include "utils/math.h"
#include "controller.h"
#include "events.h"
#include "post.h"

#define MINIMUM_CHARGE_CURRENT 6

using board::ACRelay;
using board::GFCI;
using board::J1772Pilot;
using board::J1772Status;
using event::Event;
using event::Loop;

namespace
{
    using evse::State;
    using evse::TemperatureMonitor;

    uint8_t getMaxChargeCurrent()
    {
        uint8_t amps = State::get().max_amps_target;
        const TemperatureMonitor::TemperatureState temp_state = TemperatureMonitor::getState();

        if (temp_state == TemperatureMonitor::ELEVATED)
            amps = utils::max(amps/2, MINIMUM_CHARGE_CURRENT);

        else if (temp_state == TemperatureMonitor::HIGH)
            amps = MINIMUM_CHARGE_CURRENT;

        return amps;
    }

    void setControllerState(const State::ControllerState state)
    {
        State::get().controller = state;
        Loop::post(Event(EVENT_CONTROLLER_STATE, state));
    }

    void setJ1772State(const J1772Status::J1772State j1772)
    {
        State::get().j1772 = j1772;
        Loop::post(Event(EVENT_J1772_STATE, j1772));
    }

    State::ControllerFault postEventToFaultCode(const uint32_t postEvent)
    {
        switch (postEvent)
        {
            case EVENT_POST_GFCI:
                return State::FAULT_POST_GFCI;
            case EVENT_POST_ACRELAY:
                return State::FAULT_POST_RELAY;
        }
        return State::NOTHING_WRONG;
    }
}

namespace evse
{

Controller& Controller::init()
{
    static Controller controller;
    return controller;
}

Controller::Controller()
    : acRelay(ACRelay::get())
    , gfci(GFCI::get())
    , j1772Status(J1772Status::get())
    , lastState(J1772Status::UNKNOWN)
{
    enableCharge(false);
}

Controller::~Controller()
{
}

void Controller::update()
{
    switch(State::get().controller)
    {
        case State::RUNNING:
            updateRunning();
            break;

        case State::BOOTUP:
            setControllerState(State::POST);
            POST::start();
            break;

        case State::POST:
        case State::FAULT:
            break;
    }
}

void Controller::updateRunning()
{
    State& state = State::get();

    const auto j1772 = j1772Status.read();
    if (state.j1772 == j1772)
        return; // State hasn't changed...

    switch (j1772)
    {
        case J1772Status::STATE_A:
        case J1772Status::STATE_E:
        case J1772Status::NOT_READY:
            enableCharge(false);
            J1772Pilot::set(J1772Pilot::HIGH);
            break;

        case J1772Status::STATE_B:
        case J1772Status::STATE_D:              // Vent required :(
        case J1772Status::DIODE_CHECK_FAILED:   // Keep PWM up so we can re-check
            enableCharge(false);
            updateChargeCurrent(true);
            // TODO: Debounce state changes to avoid relay clicking hell?
            break;

        case J1772Status::STATE_C:
            updateChargeCurrent(true);
            enableCharge(true);
            break;

        case J1772Status::UNKNOWN:
        case J1772Status::IMPLAUSIBLE:
            enableCharge(false);
            J1772Pilot::set(J1772Pilot::LOW);
            // TODO: Snap to error condition?
            break;
    }

    setJ1772State(j1772);
}

void Controller::onEvent(const event::Event &event)
{
    switch (event.id)
    {
        case EVENT_UPDATE:
            update();
            break;

        case EVENT_POST_GFCI:
            gfci.selfTest();
            break;

        case EVENT_POST_ACRELAY:
            acRelay.selfTest(checkEVPresent());
            break;

        case EVENT_POST_COMPLETED:
            if (event.param == 0)
            {
                J1772Pilot::set(J1772Pilot::HIGH);
                setControllerState(State::RUNNING);
            } else {
                setFault(postEventToFaultCode(event.param));
            }
            break;

        case EVENT_GFCI_TRIPPED:
            setFault(State::FAULT_GFCI_TRIPPED);
            break;

        case EVENT_MAX_AMPS_CHANGED:
            updateChargeCurrent();
            break;

        case EVENT_TEMPERATURE_ALERT:
            if (event.param == TemperatureMonitor::CRITICAL)
                setFault(State::FAULT_TEMPERATURE_CRITICAL);
            else
                updateChargeCurrent();
            break;
    }
}

bool Controller::checkEVPresent()
{
    J1772Pilot::set(J1772Pilot::HIGH);
    const bool present = (j1772Status.read() != J1772Status::STATE_A);
    J1772Pilot::set(J1772Pilot::LOW);

    return present;
}

void Controller::enableCharge(const bool enable)
{
    if (enable)
    {
        acRelay.enable();
        State::get().charge_start_time = system::Timer::millis();
    } else {
        acRelay.disable();
        State::get().charge_start_time = 0;
    }
}

void Controller::updateChargeCurrent(const bool enablePwm)
{
    const uint8_t max_amps = getMaxChargeCurrent();

    State &state = State::get();
    state.max_amps_limit = utils::min(state.max_amps_target, max_amps);

    if (enablePwm || J1772Pilot::getMode() == J1772Pilot::PWM)
        J1772Pilot::pwmAmps(max_amps);
}

void Controller::setFault(const State::ControllerFault fault)
{
    enableCharge(false);
    J1772Pilot::set(J1772Pilot::LOW);
    State::get().fault = fault;
    setControllerState(State::FAULT);
}

}
