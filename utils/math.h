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

namespace nospark {
namespace utils {

template <typename T>
T min(T a, T b) {
  if (b < a) return b;
  return a;
}

template <typename T>
T max(T a, T b) {
  if (b > a) return b;
  return a;
}

template <typename T>
constexpr T square_root(const T &val) {
  if (val < 2) return val;

  T low = 1;
  T high = val / 2 + 1;

  while ((high - low) > 1) {
    const T test = (high - low) / 2 + low;
    const T sq = test * test;

    if (sq == val)
      return test;
    else if (sq > val)
      high = test;
    else
      low = test;
  }

  return low;
}

}  // namespace utils
}  // namespace nospark
