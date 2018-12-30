/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_OUTPUT_HPP
#define __INCLUDE_MARINA_OUTPUT_HPP

#define WLR_USE_UNSTABLE

#include <GLES2/gl2.h>

#include "marina-server.hpp"

class MarinaOutput {
public:
    struct wl_listener destroy;
    static void destroy_notify(struct wl_listener* listener, void* data);
    struct wl_listener damage_destroy;
    static void damage_destroy_notify(struct wl_listener* listener, void* data);
    struct wl_listener damage_frame;
    static void damage_frame_notify(struct wl_listener* listener, void* data);

public:
    MarinaServer* server;
    struct wlr_output* wlr_output;
    struct timespec last_frame;

    struct wlr_output_damage *damage;

    struct wl_list link;

    struct wlr_renderer* renderer;

    static void scissor_output(struct wlr_output* wlr_output, struct wlr_renderer* renderer, pixman_box32_t *rect);
    void damage_whole();
    void render_output();

    MarinaOutput(struct wlr_output* wlr_output, MarinaServer* server);
    ~MarinaOutput();
};

#endif