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
#include <util/delay.h>
#include <stdbool.h>

#include "utils/pair.h"
#include "lcd1602.h"

#define LCD_I2C_ADDR            0x20

#define LCD_RS              (1 << 7)
#define LCD_RW              (1 << 6)
#define LCD_EN              (1 << 5)
#define LCD_D4_OFFSET              1

#define LCD_BL_R            (1 << 6) // A
#define LCD_BL_G            (1 << 7) // A
#define LCD_BL_B            (1 << 0) // B

#define LCD_CLEAR           (1 << 0)

#define LCD_HOME            (1 << 1)

#define LCD_ENTRY           (1 << 2)
#define LCD_ENTRY_INC       (1 << 1)
#define LCD_ENTRY_SHIFT     (1 << 0)

#define LCD_CTRL            (1 << 3)
#define LCD_CTRL_DISPLAY    (1 << 2)
#define LCD_CTRL_CURSOR     (1 << 1)
#define LCD_CTRL_BLINK      (1 << 0)

#define LCD_FN_SET          (1 << 5)
#define LCD_FN_4BIT         (1 << 4)
#define LCD_FN_2LINE        (1 << 2)
#define LCD_FN_5x11         (1 << 1)

#define LCD_SET_CG_ADDR     (1 << 6)

#define LCD_SET_DD_ADDR     (1 << 7)

// OpenEVSE LCD backpack pin map
//  B7 B6 B5 B4 B3 B2 B1 B0 A7 A6 A5 A4 A3 A2 A1 A0 - MCP23017
//  15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
//  RS RW EN D4 D5 D6 D7 B  G  R     B4 B3 B2 B1 B0

using utils::Pair;

namespace
{
    using devices::LCD16x2;

    uint8_t reverse_tuple(const uint8_t b)
    {
        static const uint8_t b_map[] =
        {
            0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
            0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF
        };
        return b_map[b & 0xF];
    }

    uint8_t data4bit(const uint8_t b)
    {
        return reverse_tuple(b) << LCD_D4_OFFSET;
    }
    
    Pair<uint8_t,uint8_t> bits4color(const LCD16x2::Backlight color)
    {
        uint8_t a = LCD_BL_R | LCD_BL_G;
        uint8_t b = LCD_BL_B;

        if (color == LCD16x2::OFF)
            return Pair<uint8_t,uint8_t>(a, b);
        
        if (color == LCD16x2::WHITE)
            return Pair<uint8_t,uint8_t>(0, 0);

        if (color == LCD16x2::RED
         || color == LCD16x2::YELLOW
         || color == LCD16x2::MAGENTA)
            a &= ~LCD_BL_R;

        if (color == LCD16x2::GREEN
         || color == LCD16x2::YELLOW
         || color == LCD16x2::CYAN)
            a &= ~LCD_BL_G;

        if (color == LCD16x2::BLUE
         || color == LCD16x2::CYAN
         || color == LCD16x2::MAGENTA)
            b &= ~LCD_BL_B;

        return Pair<uint8_t,uint8_t>(a, b);
    }
}

namespace devices
{

LCD16x2::LCD16x2()
    : io(LCD_I2C_ADDR)
    , backlight(OFF)
{
    // Initialize GPIO expander
    io.ioDir(0x1F, 0x00);
    io.pullUp(0x1F, 0x00);

    io.write(0, 0);

    // Wait for voltages to settle (LCD is touchy...)
    _delay_ms(100);

    uint8_t fn = LCD_FN_SET | LCD_FN_4BIT | LCD_FN_2LINE;

    pulse(data4bit(fn >> 4));
    _delay_us(50);

    pulse(data4bit(fn >> 4));
    _delay_us(50);

    pulse(data4bit(fn >> 4));
    _delay_us(50);

    fn &= ~LCD_FN_4BIT;

    pulse(data4bit(fn >> 4));
    _delay_us(50);

    const uint8_t ctrl = LCD_CTRL | LCD_CTRL_DISPLAY;
    writeCommand(ctrl);
    _delay_us(50);

    clear();

    writeCommand(LCD_ENTRY | LCD_ENTRY_INC);
    _delay_us(50);

    setBacklight(GREEN);
}

void LCD16x2::clear()
{
    writeCommand(LCD_CLEAR);
    _delay_ms(2);
}

void LCD16x2::home()
{
    writeCommand(LCD_HOME);
    _delay_ms(2);
}

void LCD16x2::move(const uint8_t x, const uint8_t y)
{
    const uint8_t row_offset[] = {0x00, 0x40, 0x14, 0x54};
    writeCommand(LCD_SET_DD_ADDR | (x + row_offset[y]));
    _delay_us(50);
}

void LCD16x2::setBacklight(const Backlight color)
{
    backlight = color;
    const Pair<uint8_t,uint8_t> bits = bits4color(color);
    io.write(bits.first, bits.second);
}

void LCD16x2::createChar(const uint8_t idx, const uint8_t* data, const uint8_t len)
{
    writeCommand(LCD_SET_CG_ADDR | (idx << 3));
    for (uint8_t i = 0; i != len; ++i)
        writeData(*data++);
    move(0, 0); // To reset address pointer
}

void LCD16x2::createChar_P(const uint8_t idx, const uint8_t* data, const uint8_t len)
{
    writeCommand(LCD_SET_CG_ADDR | (idx << 3));
    for (uint8_t i = 0; i != len; ++i)
        writeData(pgm_read_byte(data++));
    move(0, 0); // To reset address pointer
}

void LCD16x2::write(const char* str)
{
    while (*str)
        write(*str++);
}

void LCD16x2::write_P(const char* str)
{
    while (pgm_read_byte(str))
        write(pgm_read_byte(str++));
}

void LCD16x2::write(const uint8_t ch)
{
    writeData(ch);
}

void LCD16x2::writeCommand(const uint8_t b)
{
    pulse(data4bit(b >> 4));
    pulse(data4bit(b));
}

void LCD16x2::writeData(const uint8_t b)
{
    pulse(data4bit(b >> 4) | LCD_RS);
    pulse(data4bit(b) | LCD_RS);
}

void LCD16x2::pulse(const uint8_t b)
{
    const Pair<uint8_t,uint8_t> bits = bits4color(backlight);
    io.writeB(b | bits.second | LCD_EN);
    io.writeB(b | bits.second);
}

}
