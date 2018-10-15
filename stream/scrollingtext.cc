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

#include "stream/scrollingtext.h"
#include "system/timer.h"

#define SCROLL_DELAY_MS 300

namespace nospark {
namespace stream {

ScrollingText::ScrollingText(const uint8_t size, const uint8_t width)
    : size(size), width(width), length(0), offset(0) {
  buffer = new char[size];
}

ScrollingText::~ScrollingText() { delete[] buffer; }

void ScrollingText::clear() { length = 0; }

void ScrollingText::setWidth(const uint8_t width) { this->width = width; }

ScrollingText &ScrollingText::operator>>(OutputStream &os) {
  if (length > width) *this << " ~ ";

  if (length <= width || offset >= length) offset = 0;

  uint8_t chars = width;
  uint8_t idx = offset;
  buffer[length] = 0;

  while (chars--) {
    if (length >= width || idx < length) {
      os << buffer[idx++];
      if (length >= width && idx >= length) idx = 0;
    } else {
      os << ' ';
    }
  }

  update();
  return *this;
}

void ScrollingText::update() {
  const uint32_t now = system::Timer::millis();
  if (now - last_update > SCROLL_DELAY_MS) {
    ++offset;
    last_update = now;
  }
}

void ScrollingText::write(const char ch) {
  if (length < (size - 1)) buffer[length++] = ch;
}
}
}
