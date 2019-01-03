/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_INPUT_HPP
#define __INCLUDE_MARINA_INPUT_HPP

class MarinaInput;

#include "marina-server.hpp"
#include "marina-seat.hpp"

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

class MarinaCursor {
public:
    struct wl_listener motion;
    struct wl_listener motion_absolute;
    struct wl_listener button;

    static void motion_notify(struct wl_listener* listener, void* data);
    static void motion_absolute_notify(struct wl_listener* listener, void* data);
    static void button_notify(struct wl_listener* listener, void* data);

public:
    MarinaServer* server;
    struct wlr_cursor* wlr_cursor;
    struct wlr_xcursor_manager* xcursor_manager;

    void bind_pointer(MarinaInput* input);
    void detach_pointer(MarinaInput* input);

    MarinaCursor(MarinaServer* server, struct wlr_output_layout* layout);
    ~MarinaCursor();
};

#endif