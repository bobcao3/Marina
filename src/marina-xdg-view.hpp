/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_XDG_VIEW_HPP
#define __INCLUDE_MARINA_XDG_VIEW_HPP

#define WLR_USE_UNSTABLE

#include <GLES2/gl2.h>

#include "marina-view.hpp"
#include "marina-renderer.hpp"

class MarinaXDGView : public MarinaView {
public:
    struct wlr_xdg_surface* xdg_surface;

    struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;

    static void map_notify(struct wl_listener* listener, void* data);
    static void unmap_notify(struct wl_listener* listener, void* data);
    static void destroy_notify(struct wl_listener* listener, void* data);
    static void request_move_notify(struct wl_listener* listener, void* data);
    static void request_resize_notify(struct wl_listener* listener, void* data);

    MarinaXDGView(MarinaServer* server, struct wlr_xdg_surface* xdg_surface);
};

#endif