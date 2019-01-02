/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_SEAT_HPP
#define __INCLUDE_MARINA_SEAT_HPP

class MarinaSeat;

#include <vector>

#include "marina-server.hpp"
#include "marina-input.hpp"

#include <xkbcommon/xkbcommon.h>

class MarinaSeat {
public:
    MarinaServer* server;

    std::vector<MarinaInput*> keyboards;
    std::vector<MarinaInput*> pointers;
    std::vector<MarinaInput*> touches;

    struct wlr_seat* seat;

    void add_keyboard(MarinaInput* input);
    void add_pointer(MarinaInput* input);
    void add_touch(MarinaInput* input);
    void update_capabilities();

    MarinaSeat(MarinaServer* server, const char* name);
    ~MarinaSeat();
};

#endif