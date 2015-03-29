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

#include "heartbeat.h"
#include "pins.h"

namespace board
{

void Heartbeat::toggle()
{
    static Heartbeat heartbeat;
    heartbeat.pin = !heartbeat.pin;
}

Heartbeat::Heartbeat()
    : pin(PIN_MISO)
{
    pin.io(Pin::OUT);
}

}
