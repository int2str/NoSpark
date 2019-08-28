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

#include <stdlib.h>
#include <string.h>

#include "board/j1772pilot.h"
#include "customcharacters.h"
#include "devices/ds3231.h"
#include "devices/lcd1602.h"
#include "evse/chargemonitor.h"
#include "evse/settings.h"
#include "evse/state.h"
#include "lcdstaterunning.h"
#include "stream/time.h"
#include "strings.h"
#include "utils/bcd.h"

#define LCD_COLUMNS 16

#define DEGREE_SYMBOL static_cast<char>(0xDF)

#define PAGE_TIMEOUT 5000

using nospark::board::J1772Pilot;
using nospark::devices::DS3231;
using nospark::devices::LCD16x2;
using nospark::evse::ChargeMonitor;
using nospark::evse::EepromSettings;
using nospark::evse::Settings;
using nospark::evse::State;
using nospark::stream::BillboardText;
using nospark::stream::OutputStream;
using nospark::stream::Spaces;
using nospark::stream::Time;
using nospark::ui::CustomCharacters;

namespace {

void write_duration(OutputStream &os, const uint32_t ms) {
  if (!ms) {
    os << "--:--";
    return;
  }

  const uint32_t mins = ms / 1000 / 60;
  os << Time(mins / 60, mins % 60);
}

void write_kwh(OutputStream &os, const uint32_t wh) {
  char buffer[10] = {0};
  ltoa(wh / 1000, buffer, 10);

  os << buffer << '.' << static_cast<char>('0' + ((wh / 100) % 10)) << " kWh";
}

void write_cost(OutputStream &os, const uint8_t currency, const uint32_t cents,
                const uint32_t wh) {
  if (cents == 0) return;

  const char currencies[3] = {'$', CustomCharacters::EURO,
                              '\\'};  // Backslash = Yen
  uint32_t cost = wh * cents / 1000;

  char buffer[10] = {0};
  ltoa(cost / 100, buffer, 10);
  cost %= 100;

  os << static_cast<char>(currencies[currency]) << buffer << "."
     << static_cast<char>('0' + cost / 10)
     << static_cast<char>('0' + cost % 10);
}

uint8_t center_P(OutputStream &lcd, const char *str, const uint8_t offset = 0) {
  const uint8_t len = strlen_P(str) + offset;
  const uint8_t padding = (LCD_COLUMNS - len) / 2;
  lcd << Spaces(padding) << nospark::stream::PGM << str << Spaces(padding + 1);
  return padding;
}

}  // namespace

namespace nospark {
namespace ui {

LcdStateRunning::LcdStateRunning(stream::LcdStream &lcd)
    : LcdState(lcd),
      page(PAGE_DEFAULT),
      display_state(PAGE_TIMEOUT),
      billboard_text(10, 3) {
  CustomCharacters::loadCustomChars(lcd.getLCD());
  EepromSettings::load(settings);
}

bool LcdStateRunning::draw() {
  if (page == PAGE_DEFAULT)
    drawDefault();
  else
    drawKwhStats();

  return true;
}

void LcdStateRunning::drawDefault() {
  const State &state = State::get();
  const ChargeMonitor &chargeMonitor = ChargeMonitor::get();

  uint8_t amps = state.max_amps_limit;
  if (chargeMonitor.isCharging()) amps = chargeMonitor.chargeCurrent() / 1000;

  lcd.move(0, 0);

  DS3231 &rtc = DS3231::get();
  if (rtc.isPresent()) {
    rtc.read();
    lcd << stream::Time(rtc.hour, rtc.minute) << ' ' << stream::PAD_SPACE
        << rtc.readTemp() << DEGREE_SYMBOL;
  } else {
    switch (state.j1772) {
      case J1772Pilot::STATE_A:
        lcd << stream::PGM << STR_STATE_READY;
        break;

      case J1772Pilot::STATE_B:
        lcd << stream::PGM << STR_STATE_CONNECTED;
        break;

      case J1772Pilot::STATE_C:
        lcd << stream::PGM << STR_STATE_CHARGING;
        break;

      default:
        lcd << stream::PGM << STR_STATE_ERROR;
        break;
    }
  }

  lcd << ' ' << static_cast<char>(CustomCharacters::SEPARATOR) << ' ';

  if (amps)
    lcd << stream::PAD_SPACE << amps;
  else
    lcd << "--";
  lcd << 'A';

  if (state.ready == State::SCHEDULED)
    lcd << static_cast<char>(CustomCharacters::HOURGLASS);
  else if (state.ready == State::KWH_LIMIT)
    lcd << static_cast<char>(CustomCharacters::BATTERY1);
  else
    lcd << ' ';

  lcd.move(0, 1);
  billboard_text.clear();

  switch (state.j1772) {
    case J1772Pilot::UNKNOWN:
      break;

    case J1772Pilot::STATE_E:
      lcd.setBacklight(LCD16x2::RED);
      center_P(lcd, STR_STATE_ERROR);
      break;

    case J1772Pilot::STATE_A:
      lcd.setBacklight(LCD16x2::GREEN);
      if (chargeMonitor.chargeDuration() == 0 ||
          chargeMonitor.wattHours() == 0) {
        center_P(lcd, STR_NOT_CONNECTED);
      } else {
        billboard_text << BillboardText::ITEM(0) << stream::PGM << STR_CHARGED;
        billboard_text << BillboardText::ITEM(1);
        write_kwh(billboard_text, chargeMonitor.wattHours());
        billboard_text << BillboardText::ITEM(2);
        write_cost(billboard_text, settings.kwh_currency, settings.kwh_cost,
                   chargeMonitor.wattHours());
        billboard_text >> lcd;

        lcd << static_cast<char>(CustomCharacters::SEPARATOR);
        write_duration(lcd, chargeMonitor.chargeDuration());
      }
      break;

    case J1772Pilot::STATE_B:
      lcd.setBacklight(LCD16x2::YELLOW);
      center_P(lcd, STR_CONNECTED);
      break;

    case J1772Pilot::STATE_C: {
      lcd.setBacklight(LCD16x2::CYAN);

      billboard_text << BillboardText::ITEM(0) << stream::PGM << STR_CHARGING;
      billboard_text << BillboardText::ITEM(1);
      write_kwh(billboard_text, chargeMonitor.wattHours());
      billboard_text << BillboardText::ITEM(2);
      write_cost(billboard_text, settings.kwh_currency, settings.kwh_cost,
                 chargeMonitor.wattHours());
      billboard_text >> lcd;

      lcd << static_cast<char>(CustomCharacters::SEPARATOR);
      write_duration(lcd, chargeMonitor.chargeDuration());
      break;
    }

    case J1772Pilot::STATE_D:
      lcd.setBacklight(LCD16x2::RED);
      center_P(lcd, STR_VENT_REQUIRED);
      break;

    case J1772Pilot::DIODE_CHECK_FAILED:
      lcd.setBacklight(LCD16x2::RED);
      center_P(lcd, STR_DIODE_CHECK_FAILED);
      break;

    case J1772Pilot::NOT_READY:
    case J1772Pilot::IMPLAUSIBLE:
      break;
  }
}

void LcdStateRunning::drawKwhStats() {
  lcd.setBacklight(LCD16x2::WHITE);

  lcd.move(0, 0);

  uint16_t *p = 0;
  switch (page) {
    case PAGE_KWH_WEEK:
      lcd << stream::PGM << STR_STATS_WEEK;
      p = &settings.kwh_week;
      break;

    case PAGE_KWH_MONTH:
      lcd << stream::PGM << STR_STATS_MONTH;
      p = &settings.kwh_month;
      break;

    case PAGE_KWH_YEAR:
      lcd << stream::PGM << STR_STATS_YEAR;
      p = &settings.kwh_year;
      break;

    case PAGE_KWH_TOTAL:
      lcd << stream::PGM << STR_STATS_TOTAL;
      p = &settings.kwh_total;
      break;

    default:
      break;
  }

  lcd.move(0, 1);

  char buffer[10] = {0};
  ltoa(p ? *p : 0, buffer, 10);

  lcd << buffer << " kWh";
  write_cost(lcd, settings.kwh_currency, settings.kwh_cost, *p * 1000l);
}

void LcdStateRunning::select() {
  lcd.clear();
  if (++page == PAGE_MAX) page = PAGE_DEFAULT;

  // If we're about to display KWH stats, refresh the settings first
  if (page != PAGE_DEFAULT) EepromSettings::load(settings);
}

}  // namespace ui
}  // namespace nospark
