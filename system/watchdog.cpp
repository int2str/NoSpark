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

#include <avr/wdt.h>

#include "watchdog.h"

namespace system
{

Watchdog& Watchdog::get()
{
    static Watchdog watchdog;
    return watchdog;
}

Watchdog::Watchdog()
{
    wdt_enable(WDTO_8S);
}

void Watchdog::enable()
{
    get();
}

void Watchdog::reset()
{
    get().reset_impl();
}

void Watchdog::force_restart()
{
    wdt_enable(WDTO_15MS);
    while(1) {}
}

void Watchdog::reset_impl()
{
    wdt_reset();
}

}
