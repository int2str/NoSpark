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
#include <stdbool.h>
#include <util/delay.h>

#include "lcd1602.h"
#include "utils/pair.h"

#define LCD_I2C_ADDR 0x20

#define LCD_COMMAND_DELAY_US 40

#define LCD_RS (1 << 7)
#define LCD_RW (1 << 6)
#define LCD_EN (1 << 5)
#define LCD_D4_OFFSET 1

#define LCD_BL_R (1 << 6)  // A
#define LCD_BL_G (1 << 7)  // A
#define LCD_BL_B (1 << 0)  // B

#define LCD_CLEAR (1 << 0)

#define LCD_HOME (1 << 1)

#define LCD_ENTRY (1 << 2)
#define LCD_ENTRY_INC (1 << 1)
#define LCD_ENTRY_SHIFT (1 << 0)

#define LCD_CTRL (1 << 3)
#define LCD_CTRL_DISPLAY (1 << 2)
#define LCD_CTRL_CURSOR (1 << 1)
#define LCD_CTRL_BLINK (1 << 0)

#define LCD_FN_SET (1 << 5)
#define LCD_FN_4BIT (1 << 4)
#define LCD_FN_2LINE (1 << 2)
#define LCD_FN_5x11 (1 << 1)

#define LCD_SET_CG_ADDR (1 << 6)

#define LCD_SET_DD_ADDR (1 << 7)

// OpenEVSE LCD backpack pin map
//  B7 B6 B5 B4 B3 B2 B1 B0 A7 A6 A5 A4 A3 A2 A1 A0 - MCP23017
//  15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
//  RS RW EN D4 D5 D6 D7 B  G  R     B4 B3 B2 B1 B0

using nospark::utils::Pair;

namespace {
using nospark::devices::LCD16x2;

// This is a map of tuples, bit reversed and left shifted over
// by one to map the D4-D7 bits onto the I/O expanders "B"
// register.
const uint8_t TUPLE_MAP[] = {0x00, 0x10, 0x08, 0x18, 0x04, 0x14, 0x0c, 0x1c,
                             0x02, 0x12, 0x0a, 0x1a, 0x06, 0x16, 0x0e, 0x1e};

Pair<uint8_t, uint8_t> bits4color(const LCD16x2::BacklightType type,
                                  const LCD16x2::Backlight color) {
  uint8_t a = LCD_BL_R | LCD_BL_G;
  uint8_t b = LCD_BL_B;

  if (color == LCD16x2::OFF) return Pair<uint8_t, uint8_t>(a, b);

  if (color == LCD16x2::WHITE || type == LCD16x2::MONOCHROME)
    return Pair<uint8_t, uint8_t>(0, 0);

  if (color == LCD16x2::RED || color == LCD16x2::YELLOW ||
      color == LCD16x2::MAGENTA)
    a &= ~LCD_BL_R;

  if (color == LCD16x2::GREEN || color == LCD16x2::YELLOW ||
      color == LCD16x2::CYAN)
    a &= ~LCD_BL_G;

  if (color == LCD16x2::BLUE || color == LCD16x2::CYAN ||
      color == LCD16x2::MAGENTA)
    b &= ~LCD_BL_B;

  return Pair<uint8_t, uint8_t>(a, b);
}
}

namespace nospark {
namespace devices {

LCD16x2::LCD16x2()
    : io(LCD_I2C_ADDR), backlight_bits(0, 0), backlight_type(RGB) {
  backlight_bits = bits4color(backlight_type, OFF);

  // Initialize GPIO expander
  io.ioDir(0x1F, 0x00);
  io.pullUp(0x1F, 0x00);

  io.write(0, 0);

  // Wait for voltages to settle (LCD is touchy...)
  _delay_ms(100);

  uint8_t fn = LCD_FN_SET | LCD_FN_4BIT | LCD_FN_2LINE;
  for (uint8_t i = 0; i != 4; ++i) {
    if (i == 3) fn &= ~LCD_FN_4BIT;
    pulse(TUPLE_MAP[fn >> 4]);
    _delay_us(LCD_COMMAND_DELAY_US);
  }

  writeCommand(LCD_CTRL | LCD_CTRL_DISPLAY);
  _delay_us(LCD_COMMAND_DELAY_US);

  clear();

  writeCommand(LCD_ENTRY | LCD_ENTRY_INC);
  _delay_us(LCD_COMMAND_DELAY_US);

  setBacklight(OFF);
}

void LCD16x2::clear() {
  writeCommand(LCD_CLEAR);
  _delay_ms(2);
}

void LCD16x2::home() {
  writeCommand(LCD_HOME);
  _delay_ms(2);
}

void LCD16x2::move(const uint8_t x, const uint8_t y) {
  const uint8_t row_offset[] = {0x00, 0x40, 0x14, 0x54};
  writeCommand(LCD_SET_DD_ADDR | (x + row_offset[y]));
}

void LCD16x2::setBacklight(const Backlight color) {
  backlight_bits = bits4color(backlight_type, color);
  io.write(backlight_bits.first, backlight_bits.second);
}

void LCD16x2::setBacklightType(const BacklightType type) {
  backlight_type = type;
}

void LCD16x2::createChar(const uint8_t idx, const uint8_t *data,
                         const uint8_t len) {
  writeCommand(LCD_SET_CG_ADDR | (idx << 3));
  for (uint8_t i = 0; i != len; ++i) write(*data++);
  move(0, 0);  // To reset address pointer
}

void LCD16x2::createChar_P(const uint8_t idx, const uint8_t *data,
                           const uint8_t len) {
  writeCommand(LCD_SET_CG_ADDR | (idx << 3));
  for (uint8_t i = 0; i != len; ++i) write(pgm_read_byte(data++));
  move(0, 0);  // To reset address pointer
}

void LCD16x2::write(const uint8_t ch) {
  pulse(TUPLE_MAP[ch >> 4] | LCD_RS);
  pulse(TUPLE_MAP[ch & 0xF] | LCD_RS);
}

void LCD16x2::writeCommand(const uint8_t b) {
  pulse(TUPLE_MAP[b >> 4]);
  pulse(TUPLE_MAP[b & 0xF]);
}

void LCD16x2::pulse(const uint8_t b) {
  io.writeB(b | backlight_bits.second | LCD_EN);
  io.writeB(b | backlight_bits.second);
}
}
}
