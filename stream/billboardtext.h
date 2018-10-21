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

namespace nospark {
namespace stream {

class BillboardText : public OutputStream {
 public:
  struct ITEM {
    ITEM(uint8_t item) : item(item) {}
    const uint8_t item;
  };

  BillboardText(const uint8_t width, const uint8_t items);
  ~BillboardText();

  void clear();

  BillboardText &operator>>(OutputStream &os);
  OutputStream &operator<<(const ITEM &item);

 private:
  void update();

  virtual void write(const char ch);

  uint32_t last_update_;

  const uint8_t width_;
  const uint8_t items_;
  uint8_t current_item_;
  uint8_t insert_position_;
  char *buffer_;
};

}  // namespace stream
}  // namespace nospark
