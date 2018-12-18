/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_INPUT_HPP
#define __INCLUDE_MARINA_INPUT_HPP

#include "marina-server.hpp"

#include <xkbcommon/xkbcommon.h>

class MarinaInput {
public:
    struct wl_listener destroy;
    struct wl_listener keyboard_key;

    static void destroy_notify(struct wl_listener* listener, void* data);
    static void keyboard_key_notify(struct wl_listener* listener, void* data);

public:
    MarinaServer* server;
    struct wlr_input_device* device;

    MarinaInput(MarinaServer* server, struct wlr_input_device* device);
    ~MarinaInput();
};

#endif