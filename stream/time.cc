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

#include "stream/time.h"

namespace nospark {
namespace stream {

Time::Time(const uint8_t hh, const uint8_t mm) : hh(hh), mm(mm) {}

OutputStream &operator<<(OutputStream &out, const Time &time) {
  return out << stream::PAD_ZERO << time.hh << ':' << stream::PAD_ZERO
             << time.mm;
}

}  // namespace stream
}  // namespace nospark
