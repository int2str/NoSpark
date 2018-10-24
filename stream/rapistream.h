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

#include "serial/usart.h"
#include "stream/uartstream.h"

namespace nospark {
namespace stream {

class RapiStream : public OutputStream {
 public:
  struct OK {};
  struct ERROR {};
  struct END {};

  RapiStream(UartStream &uart);

  RapiStream &operator<<(const Flags flags);
  RapiStream &operator<<(const uint8_t val);
  RapiStream &operator<<(const uint32_t val);
  RapiStream &operator<<(const char *str);

  RapiStream &operator<<(const OK &ok);
  RapiStream &operator<<(const ERROR &error);
  RapiStream &operator<<(const END &end);

 private:
  UartStream &uart_;
  uint8_t checksum_;
  bool first_parameter_;

  void insertSpaceBeforeFirstParam();

  void write(const char ch) override;
};

}  // namespace stream
}  // namespace nospark
