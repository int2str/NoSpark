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

#include "utils/cpp.h"
#include "utils/queue.h"
#include "event.h"
#include "handler.h"

namespace event
{

// Basic event loop driver.
// |dispatch| must be called regularly in order to distribute events.
// This is a singleton; there shall be only one.
class Loop
{
    Loop();

public:
    // Posts to all registered handlers on the next call to |dispatch|
    static void post(const Event &e);

    // Posts to all registered handlers when |dispatch| is called after
    // |ms| have elapsed
    static void postDelayed(const Event &e, const uint32_t ms);

    // Must be called regularly to dispatch events
    static void dispatch();

protected:
    friend class Handler;
    static void addHandler(Handler *ph);

private:
    static Loop &get();
    void dispatch_impl();

    utils::Queue<Event> q_event;
    utils::Queue<Handler*> q_handler;

    DISALLOW_COPY_AND_ASSIGN(Loop);
};

}
