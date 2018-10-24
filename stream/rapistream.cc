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

#include "stream/rapistream.h"

namespace {

char hex_ch(uint8_t val) {
  if (val < 10) return '0' + val;
  if (val < 16) return 'A' + val - 10;
  return '?';
}

void appendChecksum(nospark::stream::UartStream& uart, uint8_t checksum) {
  uart << '^' << hex_ch(checksum >> 4) << hex_ch(checksum & 0xF) << '\r';
}

}  // namespace

namespace nospark {
namespace stream {

RapiStream::RapiStream(UartStream& uart)
    : uart_(uart), checksum_(0), first_parameter_(false) {}

void RapiStream::write(const char ch) {
  checksum_ ^= ch;
  uart_ << ch;
}

void RapiStream::insertSpaceBeforeFirstParam() {
  if (first_parameter_) {
    OutputStream::operator<<(' ');
    first_parameter_ = false;
  }
}

RapiStream& RapiStream::operator<<(const Flags flags) {
  OutputStream::operator<<(flags);
  return *this;
}

RapiStream& RapiStream::operator<<(const uint8_t val) {
  insertSpaceBeforeFirstParam();
  OutputStream::operator<<(val);
  return *this;
}

RapiStream& RapiStream::operator<<(const uint32_t val) {
  insertSpaceBeforeFirstParam();
  OutputStream::operator<<(val);
  return *this;
}

RapiStream& RapiStream::operator<<(const char* str) {
  insertSpaceBeforeFirstParam();
  OutputStream::operator<<(str);
  return *this;
}

RapiStream& RapiStream::operator<<(const OK&) {
  checksum_ = 0;
  first_parameter_ = false;
  *this << "$OK";
  first_parameter_ = true;
  return *this;
}

RapiStream& RapiStream::operator<<(const ERROR&) {
  checksum_ = 0;
  first_parameter_ = false;
  *this << "$NK";
  first_parameter_ = true;
  return *this;
}

RapiStream& RapiStream::operator<<(const END&) {
  appendChecksum(uart_, checksum_);
  return *this;
}

}  // namespace stream
}  // namespace nospark
