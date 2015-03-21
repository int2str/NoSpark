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

#include "event/loop.h"
#include "events.h"
#include "post.h"

using event::Event;
using event::Loop;

namespace
{
    const uint8_t postEvents[] = {
        EVENT_POST_GFCI,
//        EVENT_POST_ACRELAY,
        0
    };
}

namespace evse
{

POST& POST::get()
{
    static POST post;
    return post;
}

POST::POST()
    : current(0)
{
}

void POST::start()
{
    get().current = 0;
    Loop::post(Event(EVENT_POST_BEGIN));
}

void POST::onEvent(const event::Event& event)
{
    switch (event.id)
    {
        case EVENT_POST_SUCCESS:
            ++current;
            // Fall-through intended
        case EVENT_POST_BEGIN:
            if (postEvents[current]) {
                Loop::post(Event(postEvents[current]));
            } else {
                Loop::post(Event(EVENT_POST_COMPLETED, 0));
            }
            break;

        case EVENT_POST_FAILED:
            Loop::post(Event(EVENT_POST_COMPLETED, postEvents[current]));
            break;
    }
}

}
