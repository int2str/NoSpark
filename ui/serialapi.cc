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

using nospark::board::J1772Pilot;
using nospark::event::Event;
using nospark::event::Loop;
using nospark::evse::ChargeMonitor;
using nospark::evse::EepromSettings;
using nospark::evse::Settings;
using nospark::evse::State;
using nospark::devices::DS3231;
using nospark::serial::Usart;
using nospark::stream::RapiStream;

namespace {

// Save on event change somehow? Simplify code?
// Un-dupe? :)
void saveMaxAmps(const uint8_t max_amps) {
  Settings settings;
  EepromSettings::load(settings);
  settings.max_current = max_amps;
  EepromSettings::save(settings);
}

constexpr uint16_t NOT_FOUND = -1;

uint16_t find_char(const char *buffer, char ch) {
  const char *it = buffer;
  while (*it && *it != ch) ++it;
  if (*it == 0) return NOT_FOUND;
  return it - buffer;
}

// TODO: Move hex_ functions to utils somewhere,
// or incorporate into stream::?
char hex_ch(uint8_t val) {
  if (val < 10) return '0' + val;
  if (val < 16) return 'A' + val - 10;
  return '?';
}

uint8_t hex_val(char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'A') return ch - 'A' + 10;
  return 0;
}

bool verifyChecksum(const char *buffer, const uint8_t len) {
  uint16_t checksum_offset = find_char(buffer, '^');
  if (checksum_offset == NOT_FOUND) return true;
  if (checksum_offset >= (uint16_t)(len - 2)) return false;

  uint8_t checksum_in = hex_val(buffer[checksum_offset + 1]) * 16 +
                        hex_val(buffer[checksum_offset + 2]);

  uint8_t checksum_calc = 0;
  while (*buffer != '^') checksum_calc ^= *buffer++;

  return checksum_calc == checksum_in;
}

uint32_t getElapsed() {
  ChargeMonitor &cm = ChargeMonitor::get();
  if (!cm.isCharging()) return 0;
  return cm.chargeDuration() / 1000;
}

uint8_t getMaxAmps() {
  State &state = State::get();
  return state.max_amps_limit;
}

uint32_t getChargingMilliamps() {
  ChargeMonitor &cm = ChargeMonitor::get();
  if (!cm.isCharging()) return 0;
  return cm.chargeCurrent();
}

uint32_t getChargingWattseconds() {
  ChargeMonitor &cm = ChargeMonitor::get();
  if (!cm.isCharging()) return 0;
  return cm.wattSeconds();
}

uint32_t getTotalKwh() {
  Settings settings;
  EepromSettings::load(settings);
  return settings.kwh_total;
}

uint8_t getKwhLimit() {
  Settings settings;
  EepromSettings::load(settings);
  return settings.kwh_limit;
}

uint8_t getTemperature() {
  DS3231 &rtc = DS3231::get();
  if (!rtc.isPresent()) return 0;
  return rtc.readTemp();
}

// Map the EVSE and J1772 to the RAPI states
uint8_t mapState() {
  State &state = State::get();

  // J1772 states map pretty much directly
  if (state.controller == State::RUNNING) {
    if (state.j1772 < J1772Pilot::STATE_E) return state.j1772;
    if (state.j1772 == J1772Pilot::DIODE_CHECK_FAILED) return 0x05;
    // Somehow, STATE_E is not mapped in the RAPI protocol?
    return 0;
  }

  switch (state.fault) {
    case State::FAULT_GFCI_TRIPPED:
      return 0x06;
    case State::FAULT_RELAY_NO_GROUND:
      return 0x07;
    case State::FAULT_RELAY_STUCK:
      return 0x08;
    case State::FAULT_POST_GFCI:
      return 0x09;
    case State::FAULT_TEMPERATURE_CRITICAL:
      return 0x0A;

    // The above check should prevent us from getting here...
    case State::NOTHING_WRONG:
      break;
  }

  // TODO: Implement additional states
  // 0x0B = over-current
  // 0xFE = sleeping
  // 0xFF = disabled

  return 0;  // UNKNOWN
}

}  // namespace

namespace nospark {
namespace ui {

SerialApi::SerialApi(stream::UartStream &uart) : rapi(uart) {}

void SerialApi::handleGet(const char *buffer) {
  switch (buffer[2]) {  // Sub-command
    case GET_VERSION:
      rapi << RapiStream::OK() << stream::PGM << STR_NOSPARK_VER << " "
           << stream::PGM << RAPI_VERSION;
      break;

    case GET_AMMETER_CALIBRATION:
      // TODO: Hard-coded for now, ultimately, needs to be stored in settings?
      rapi << RapiStream::OK() << "200 0";
      break;

    case GET_STATE:
      rapi << RapiStream::OK() << mapState() << " " << getElapsed();
      break;

    case GET_CURRENT:
      rapi << RapiStream::OK() << getChargingMilliamps() << " -1";
      break;

    case GET_CHARGE_STATS:
      rapi << RapiStream::OK() << getChargingWattseconds() << " "
           << getTotalKwh() * 1000;
      break;

    case GET_CURRENT_AND_FLAGS:
      // TODO: Flags hard-coded; move to settings for SET command to work
      rapi << RapiStream::OK() << getMaxAmps() << " "
           << "0021";
      break;

    case GET_MIN_MAX_AMPS:
      rapi << RapiStream::OK() << "6 80";
      break;

    case GET_CHARGE_TIMER:
      // TODO: Populate...
      rapi << RapiStream::OK() << "0 0 0 0";
      break;

    case GET_KWH_LIMIT:
      rapi << RapiStream::OK() << getKwhLimit();
      break;

    case GET_TEMPERATURE:
      rapi << RapiStream::OK() << (uint8_t)(getTemperature() * 10)
           << " -2560 -2560";
      break;

    case GET_FAULT_COUNTERS:
      // TODO: Seems like something we should track ;)
      // Order is GFI, No Ground, Stuck Relay
      rapi << RapiStream::OK() << "0 0 0";
      break;

    default:
      rapi << RapiStream::ERROR();
      break;
  }

  rapi << RapiStream::END();
}

void SerialApi::handleSet(const char *) {}
void SerialApi::handleFunction(const char *) {}

bool SerialApi::handleCommand(const char *buffer, const uint8_t len) {
  if (!verifyChecksum(buffer, len) || len < 3) return true;

  switch (buffer[1]) {  // Command type
    case COMMAND_TYPE_GET:
      handleGet(buffer);
      break;
    case COMMAND_TYPE_SET:
      handleSet(buffer);
      break;
    case COMMAND_TYPE_FUNCTION:
      handleFunction(buffer);
      break;
    default:
      rapi << RapiStream::ERROR() << RapiStream::END();
      break;
  }

  return true;
}

}  // namespace ui
}  // namespace nospark
