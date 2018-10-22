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
#include "events.h"
#include "evse/chargemonitor.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "system/watchdog.h"
#include "ui/serialapi.h"
#include "ui/serialprotocol.h"
#include "ui/strings.h"

#define PARAM_NOT_FOUND 0xFFFF

using nospark::event::Event;
using nospark::event::Loop;
using nospark::evse::ChargeMonitor;
using nospark::evse::EepromSettings;
using nospark::evse::Settings;
using nospark::evse::State;
using nospark::devices::DS3231;
using nospark::serial::Usart;

namespace {
// Save on event change somehow? Simplify code?
// Un-dupe? :)
void saveMaxAmps(const uint8_t max_amps) {
  Settings settings;
  EepromSettings::load(settings);
  settings.max_current = max_amps;
  EepromSettings::save(settings);
}

uint16_t paramGet(const char *buffer, const char param) {
  bool found = false;
  uint16_t val = PARAM_NOT_FOUND;

  while (*buffer) {
    const char ch = *buffer++;
    if (found) {
      if (ch < '0' || ch > '9') break;
      val *= 10;
      val += ch - '0';
    }

    if (ch == param) {
      val = 0;
      found = true;
    }
  }

  return val;
}

void paramAdd(char *buffer, const char param, uint32_t value) {
  // Find end of string
  while (*buffer) ++buffer;

  // Insert parameter
  *buffer++ = ' ';
  *buffer++ = param;

  // Convert value into buffer
  ltoa(value, buffer, 10);
}

void cmdGetStatus(char *response) {
  State &state = State::get();

  paramAdd(response, 'J', state.j1772);
  paramAdd(response, 'R', state.ready);

  DS3231 &rtc = DS3231::get();
  if (rtc.isPresent()) paramAdd(response, 'T', rtc.readTemp());
}

void cmdGetChargeStatus(char *response) {
  ChargeMonitor &cm = ChargeMonitor::get();

  const uint8_t charging = !!cm.isCharging();
  paramAdd(response, 'C', charging);
  if (charging) paramAdd(response, 'A', cm.chargeCurrent());
  paramAdd(response, 'D', cm.chargeDuration() / 1000);
  paramAdd(response, 'W', cm.wattHours());

  DS3231 &rtc = DS3231::get();
  if (rtc.isPresent()) paramAdd(response, 'T', rtc.readTemp());
}

void cmdGetKwhStats(char *response) {
  Settings settings;
  EepromSettings::load(settings);
  paramAdd(response, 'W', settings.kwh_week);
  paramAdd(response, 'M', settings.kwh_month);
  paramAdd(response, 'Y', settings.kwh_year);
  paramAdd(response, 'T', settings.kwh_total);
}

uint8_t cmdSetCurrent(const char *buffer) {
  const uint8_t amps = paramGet(buffer, 'A');
  if (amps == 0 || amps == 0xFF) return INVALID_PARAMETER;

  State::get().max_amps_target = amps;
  saveMaxAmps(amps);
  Loop::post(Event(EVENT_MAX_AMPS_CHANGED, amps));

  return OK;
}

uint8_t cmdSetReadyState(const char *buffer) {
  const uint8_t ready = paramGet(buffer, 'S');
  State &state = State::get();

  if (ready == State::KWH_LIMIT) return INVALID_PARAMETER;

  state.ready = ready == 0 ? State::READY : State::MANUAL_OVERRIDE;
  Loop::post(Event(EVENT_READY_STATE_CHANGED, state.ready));
  return OK;
}

void cmdGetTime(char *response) {
  DS3231 &rtc = DS3231::get();
  rtc.read();

  paramAdd(response, 'H', rtc.hour);
  paramAdd(response, 'M', rtc.minute);
  paramAdd(response, 'S', rtc.second);
}

void cmdSetTime(const char *buffer) {
  uint8_t h = paramGet(buffer, 'H');
  uint8_t m = paramGet(buffer, 'M');
  uint8_t s = paramGet(buffer, 'S');

  DS3231 &rtc = DS3231::get();
  rtc.read();

  if (h < 24) rtc.hour = h;

  if (m < 60) rtc.minute = m;

  if (s < 60) rtc.second = s;

  rtc.write();
}

void cmdGetDate(char *response) {
  DS3231 &rtc = DS3231::get();
  rtc.read();

  paramAdd(response, 'D', rtc.day);
  paramAdd(response, 'M', rtc.month);
  paramAdd(response, 'Y', rtc.year);
  paramAdd(response, 'W', rtc.weekday);
}

void cmdSetDate(const char *buffer) {
  DS3231 &rtc = DS3231::get();
  rtc.read();

  uint8_t p = paramGet(buffer, 'D');
  if (p > 0 && p < 32) rtc.day = p;

  p = paramGet(buffer, 'M');
  if (p > 0 && p < 13) rtc.minute = p;

  p = paramGet(buffer, 'Y');
  if (p < 100) rtc.year = p;

  p = paramGet(buffer, 'W');
  if (p > 0 && p < 8) rtc.weekday = p;
}

void cmdGetTimer(char *response) {
  Settings settings;
  EepromSettings::load(settings);

  paramAdd(response, 'A', settings.charge_timer_enabled);
  paramAdd(response, 'S', settings.charge_timer_t1);
  paramAdd(response, 'F', settings.charge_timer_t2);
}

uint8_t cmdSetTimer(const char *buffer) {
  Settings settings;
  EepromSettings::load(settings);

  uint16_t p = paramGet(buffer, 'A');
  if (p != PARAM_NOT_FOUND) settings.charge_timer_enabled = p;

  p = paramGet(buffer, 'S');
  if (p != PARAM_NOT_FOUND) {
    if ((p >> 8) > 23 || (p & 0xFF) > 59) return INVALID_PARAMETER;
    settings.charge_timer_t1 = p;
  }

  p = paramGet(buffer, 'F');
  if (p != PARAM_NOT_FOUND) {
    if ((p >> 8) > 23 || (p & 0xFF) > 59) return INVALID_PARAMETER;
    settings.charge_timer_t2 = p;
  }

  EepromSettings::save(settings);
  return OK;
}

void cmdGetChargeLimit(char *response) {
  Settings settings;
  EepromSettings::load(settings);

  paramAdd(response, 'K', settings.kwh_limit);
}

uint8_t cmdSetChargeLimit(const char *buffer) {
  Settings settings;
  EepromSettings::load(settings);

  uint16_t p = paramGet(buffer, 'K');
  if (p != PARAM_NOT_FOUND) {
    settings.kwh_limit = p;
    EepromSettings::save(settings);
  }
  return OK;
}

void cmdGetKwhCost(char *response) {
  Settings settings;
  EepromSettings::load(settings);

  paramAdd(response, 'C', settings.kwh_currency);
  paramAdd(response, 'P', settings.kwh_cost);
}

uint8_t cmdSetKwhCost(const char *buffer) {
  Settings settings;
  EepromSettings::load(settings);

  uint16_t c = paramGet(buffer, 'C');
  if (c < 3) settings.kwh_currency = c;

  uint16_t p = paramGet(buffer, 'P');
  if (p != PARAM_NOT_FOUND) settings.kwh_cost = p;

  if (c < 3 || p != PARAM_NOT_FOUND) EepromSettings::save(settings);

  return OK;
}

constexpr uint16_t NOT_FOUND = -1;

uint16_t find_char(const char *buffer, char ch) {
  const char *it = buffer;
  while (*it && *it != ch) ++it;
  if (*it == 0) return NOT_FOUND;
  return it - buffer;
}

uint8_t hex_val(char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'A') return ch - 'A' + 10;
  return 0;
}

char hex_ch(uint8_t val) {
  if (val < 10) return '0' + val;
  if (val < 16) return 'A' + val - 10;
  return '?';
}

bool verifyChecksum(const char *buffer) {
  uint16_t checksum_offset = find_char(buffer, '^');
  if (checksum_offset == NOT_FOUND) return true;

  uint8_t checksum_in = hex_val(buffer[checksum_offset + 1]) * 16 +
                        hex_val(buffer[checksum_offset + 2]);

  uint8_t checksum_calc = 0;
  while (*buffer != '^') checksum_calc ^= *buffer++;

  return checksum_calc == checksum_in;
}

}  // namespace

namespace nospark {
namespace ui {

SerialApi::SerialApi(stream::UartStream &uart) : uart(uart) {}

void SerialApi::handleGet(const char *buffer) {
  switch (buffer[2]) {
    case 'V':  // Send version info
      sendOk("N1.0.0 4.0.1");
      break;

    default:
      sendError();
      break;
  }
}

void SerialApi::handleSet(const char *buffer) {}
void SerialApi::handleFunction(const char *buffer) {}

bool SerialApi::handleCommand(const char *buffer, const uint8_t) {
  if (!verifyChecksum(buffer)) {
    sendError();
    return true;
  }

  if (buffer[1] == 'G') handleGet(buffer);

  return true;
}

void SerialApi::sendWithChecksum(const char *buffer, uint8_t seed) {
  uart << buffer;
  uint8_t checksum = seed;
  while (*buffer) checksum ^= *buffer++;
  uart << "^" << hex_ch(checksum >> 4) << hex_ch(checksum & 0xF) << CR;
}

void SerialApi::sendOk(const char *params) {
  if (params) {
    uart << "$OK ";
    sendWithChecksum(params);  // As luck would have it, XOR for "$OK " = 0!
  } else {
    sendWithChecksum("$OK");
  }
}

void SerialApi::sendError() { sendWithChecksum("$NK"); }

}  // namespace ui
}  // namespace nospark
