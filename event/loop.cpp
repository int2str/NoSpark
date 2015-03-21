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
    get().q_event.push(event);
}

void Loop::addHandler(Handler *ph)
{
    get().q_handler.push(ph);
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
    while (!q_event.empty())
    {
        const Event& event = q_event.front();
        Handler* handler = q_handler.front();

        while (1)
        {
            handler->onEvent(event);
            if (!q_handler.has_next())
            break;
            handler = q_handler.next();
        }

        q_event.pop();
    }
}

}
