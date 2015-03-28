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

#include "loop.h"

namespace event
{

void Loop::post(const Event &event)
{
    auto& events = get().events;
    events.push(event);
}

void Loop::addHandler(Handler *ph)
{
    auto& handlers = get().handlers;
    handlers.push(ph);
}

void Loop::dispatch()
{
    get().dispatch_impl();
}

Loop& Loop::get()
{
    static Loop* i = 0;
    if (i == 0)
        i = new Loop();
    return *i;
}

Loop::Loop()
{
}

void Loop::dispatch_impl()
{
    while (!events.empty())
    {
        auto event = events.front();
        for (auto handler : handlers)
            handler->onEvent(event);
        events.pop();
    }
}

}
