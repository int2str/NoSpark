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

#include <avr/pgmspace.h>
#include <stdlib.h>

#include "devices/ds3231.h"
#include "event/loop.h"
#include "evse/chargemonitor.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "system/watchdog.h"
#include "serialapi.h"
#include "serialprotocol.h"
#include "events.h"

using event::Event;
using evse::ChargeMonitor;
using evse::EepromSettings;
using evse::Settings;
using evse::State;
using devices::DS3231;
using serial::Usart;

namespace
{
    // Save on event change somehow? Simplify code?
    // Un-dupe? :)
    void saveMaxAmps(const uint8_t max_amps)
    {
        Settings settings;
        EepromSettings::load(settings);
        settings.max_current = max_amps;
        EepromSettings::save(settings);
    }

    uint8_t paramU8(const char *buffer, const char param)
    {
        bool found = false;
        uint8_t val = 0xFF;

        while (*buffer != 0)
        {
            const char ch = *buffer++;
            if (found)
            {
                if (ch < '0' || ch > '9')
                    break;
                val *= 10;
                val += ch - '0';
            }

            if (ch == param)
            {
                val = 0;
                found = true;
            }
        }

        return val;
    }

    void paramAdd(char *buffer, const char param, uint32_t value)
    {
        // Find end of string
        while (*buffer)
            ++buffer;

        // Insert parameter
        *buffer++ = ' ';
        *buffer++ = param;

        // Convert value into buffer
        ltoa(value, buffer, 10);
    }

    void cmdGetStatus(char *response)
    {
        DS3231 &rtc = DS3231::get();
        State &state = State::get();

        paramAdd(response, 'J', state.j1772);
        paramAdd(response, 'T', rtc.readTemp());
        paramAdd(response, 'R', state.ready);
    }

    void cmdGetChargeStatus(char *response)
    {
        ChargeMonitor &cm = ChargeMonitor::get();

        const uint8_t charging = !!cm.isCharging();
        paramAdd(response, 'C', charging);
        if (charging)
            paramAdd(response, 'A', cm.chargeCurrent());
        paramAdd(response, 'D', cm.chargeDuration());
        paramAdd(response, 'W', cm.wattHours());
    }

    uint8_t cmdSetCurrent(const char *buffer)
    {
        const uint8_t amps = paramU8(buffer, 'A');
        if (amps == 0 || amps == 0xFF)
            return INVALID_PARAMETER;

        State::get().max_amps_target = amps;
        saveMaxAmps(amps);
        event::Loop::post(Event(EVENT_MAX_AMPS_CHANGED, amps));

        return OK;
    }

    uint8_t cmdSetReadyState(const char *buffer)
    {
        const uint8_t ready = paramU8(buffer, 'S');
        State &state = State::get();

        if (ready == State::KWH_LIMIT)
            return INVALID_PARAMETER;

        state.ready = ready == 0 ? State::READY : State::MANUAL_OVERRIDE;
        event::Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));
        return OK;
    }

    void cmdGetTime(char *response)
    {
        DS3231 &rtc = DS3231::get();
        rtc.read();

        paramAdd(response, 'H', rtc.hour);
        paramAdd(response, 'M', rtc.minute);
        paramAdd(response, 'S', rtc.second);
    }

    void cmdSetTime(const char *buffer)
    {
        DS3231 &rtc = DS3231::get();
        rtc.read();

        uint8_t p = paramU8(buffer, 'H');
        if (p < 24)
            rtc.hour = p;

        p = paramU8(buffer, 'M');
        if (p < 60)
            rtc.minute = p;

        p = paramU8(buffer, 'S');
        if (p < 60)
            rtc.second = p;

        rtc.write();
    }

    void cmdSetDate(const char *buffer)
    {
        DS3231 &rtc = DS3231::get();
        rtc.read();

        uint8_t p = paramU8(buffer, 'D');
        if (p > 0 && p < 32)
            rtc.day = p;

        p = paramU8(buffer, 'M');
        if (p > 0 && p < 13)
            rtc.minute = p;

        p = paramU8(buffer, 'Y');
        if (p < 199)
            rtc.year = p;

        p = paramU8(buffer, 'W');
        if (p > 0 && p < 8)
            rtc.weekday = p;
    }

    void cmdGetDate(char *response)
    {
        DS3231 &rtc = DS3231::get();
        rtc.read();

        paramAdd(response, 'D', rtc.day);
        paramAdd(response, 'M', rtc.month);
        paramAdd(response, 'Y', rtc.year);
        paramAdd(response, 'W', rtc.weekday);
    }
}

namespace ui
{

SerialApi::SerialApi(serial::Usart &uart)
    : uart(uart)
{
}

void SerialApi::onEvent(const event::Event&)
{
}

bool SerialApi::handleCommand(const char *buffer, const uint8_t)
{
    const uint8_t e = paramU8(buffer, 'E');
    char response[64] = {0};
    uint8_t err = OK;

    switch (e)
    {
        case CMD_GET_STATE:
            cmdGetStatus(response);
            break;

        case CMD_GET_CHARGE_STATE:
            cmdGetChargeStatus(response);
            break;

        case CMD_GET_MAX_CURRENT:
            paramAdd(response, 'A', State::get().max_amps_target);
            paramAdd(response, 'L', State::get().max_amps_limit);
            break;

        case CMD_SET_MAX_CURRENT:
            err = cmdSetCurrent(buffer);
            break;

        case CMD_GET_TIME:
            cmdGetTime(response);
            break;

        case CMD_SET_TIME:
            cmdSetTime(buffer);
            break;

        case CMD_GET_DATE:
            cmdGetDate(response);
            break;

        case CMD_SET_DATE:
            cmdSetDate(buffer);
            break;

        case CMD_SET_SLEEP:
            event::Loop::post(Event(EVENT_REQUEST_SLEEP, paramU8(buffer, 'S')));
            break;

        case CMD_SET_READY_STATE:
            err = cmdSetReadyState(buffer);
            break;

        case CMD_GET_SAPI_VERSION:
            paramAdd(response, 'V', SAPI_VERSION);
            break;

        default:
            err = UNKNOWN_COMMAND;
            break;
    }

    if (err == OK)
    {
        uart.write("$OK");
        uart.writeln(response);
    } else {
        char error[] = "$ERR 0";
        error[5] += err;
        uart.writeln(error);
    }

    return true;
}

}
