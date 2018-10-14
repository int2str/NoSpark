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

#include <avr/eeprom.h>

namespace nospark {
namespace utils {

template <class T> class Eeprom {
public:
  static T load(const uint8_t offset) {
    T t;
    eeprom_read_block(&t, reinterpret_cast<const void *>(offset), sizeof(T));
    return t;
  }

  static void save(const uint8_t offset, const T &data) {
    eeprom_write_block(&data, reinterpret_cast<const void *>(offset),
                       sizeof(T));
  }
};
}
}
