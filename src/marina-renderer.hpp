/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_RENDERER_HPP
#define __INCLUDE_MARINA_RENDERER_HPP

#define WLR_USE_UNSTABLE

#include <GLES2/gl2.h>

#include "marina-server.hpp"
#include "marina-view.hpp"

class MarinaRenderer {
public:
    struct wlr_output* output;
    struct wlr_renderer* renderer;
    MarinaView* view;
    struct timespec* when;

    static void render_surface(struct wlr_surface* surface, int sx, int sy, void* data);
};

#endif