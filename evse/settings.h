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

namespace nospark {
namespace evse {

class Settings {
public:
  uint16_t marker;
  uint8_t revision;

  // Rev 1
  uint8_t max_current;

  // Rev 2
  uint16_t kwh_total;
  uint16_t kwh_year;
  uint16_t kwh_month;
  uint16_t kwh_week;
  uint16_t kwh_index;

  // Rev 3
  uint8_t kwh_limit;
  uint8_t sleep_mode;

  // Rev 4
  bool charge_timer_enabled;
  uint16_t charge_timer_t1;
  uint16_t charge_timer_t2;

  // Rev 5
  uint8_t kwh_currency;
  uint16_t kwh_cost;
  uint16_t ammeter_factor;
  uint16_t ammeter_offset;

  // Rev 6
  uint8_t lcd_type;

  Settings();

  void defaults();
  void upgrade();
  void postLoad();
  void preSave();
};

class EepromSettings {
public:
  static void load(Settings &settings);
  static void save(Settings &settings);
};
}
}
