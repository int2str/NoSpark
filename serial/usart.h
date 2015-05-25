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
#include <stdbool.h>

#include "utils/cpp.h"

namespace serial
{

// Non-interrupt, polling based implementation. Uses UART0.
// This is a singleton; there shall be only one.
class Usart
{
public:
    static Usart& get();

    void write(const uint8_t b);
    void write(const char *str);
    void writeln(const char *str);

    void write_P(const char *str);
    void writeln_P(const char *str);

    bool avail() const;
    uint8_t read();

private:
    Usart();

    DISALLOW_COPY_AND_ASSIGN(Usart);
};

}
