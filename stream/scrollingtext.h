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

#include "stream/outputstream.h"

namespace stream
{

class ScrollingText : public OutputStream
{
public:
    ScrollingText(const uint8_t size, const uint8_t width);
    ~ScrollingText();

    void setWidth(const uint8_t width);
    void clear();

    ScrollingText &operator>> (OutputStream &os);

private:
    void update();

    void write(const char ch) override;
    void write(const char *str) override;

    uint32_t last_update;

    const uint8_t size;
    uint8_t width;
    uint8_t length;
    uint8_t offset;
    char *buffer;
};

}
