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

#include <stdlib.h>

namespace utils
{

template <class T, size_t WINDOW>
class MovingAverage
{
public:
    MovingAverage()
        : buffer{0}
        , size(WINDOW)
        , index(0)
        , items(0)
        , sum(0)
    {
    }

    void push(const T val)
    {
        sum -= buffer[index];
        sum += val;
        buffer[index] = val;
        if (++index == size)
            index = 0;
        if (items < size)
            ++items;
    }

    T get() const
    {
        // Make sure we have sufficient data
        // to avoid initial spikes
        if (items < (size/2))
            return 0;
        return sum / items;
    }

    void clear()
    {
        sum = 0;
        items = 0;
    }

private:
    T buffer[WINDOW];
    const size_t size;
    size_t index;
    size_t items;
    T sum;
};

}
