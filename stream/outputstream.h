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

#include <stddef.h>
#include <stdint.h>

namespace nospark {
namespace stream {

enum Flags { PGM, PAD_ZERO, PAD_SPACE };

struct Spaces {
  Spaces(const size_t num) : num(num) {}
  const size_t num;
};

class OutputStream {
public:
  OutputStream();
  virtual ~OutputStream();

  OutputStream &operator<<(const Flags flags);
  OutputStream &operator<<(const Spaces spaces);
  OutputStream &operator<<(const char ch);
  OutputStream &operator<<(const char *str);
  OutputStream &operator<<(const uint8_t val);
  OutputStream &operator<<(const uint32_t val);

protected:
  virtual void write(const char ch) = 0;

private:
  uint8_t flags;
};
}
}
