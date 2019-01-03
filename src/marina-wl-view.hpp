/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_WL_VIEW_HPP
#define __INCLUDE_MARINA_WL_VIEW_HPP

#define WLR_USE_UNSTABLE

#include <GLES2/gl2.h>

#include "marina-view.hpp"
#include "marina-renderer.hpp"

class MarinaWLView : public MarinaView {
public:
    struct wlr_wl_shell_surface* wl_shell_surface;

	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;

    static void destroy_notify(struct wl_listener* listener, void* data);
    static void request_move_notify(struct wl_listener* listener, void* data);
    static void request_resize_notify(struct wl_listener* listener, void* data);

    MarinaWLView(MarinaServer* server, struct wlr_wl_shell_surface* wl_shell_surface);
};

#endif