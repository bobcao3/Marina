/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_VIEW_HPP
#define __INCLUDE_MARINA_VIEW_HPP

#define WLR_USE_UNSTABLE

#include "marina-server.hpp"

enum MarinaViewType {
    UNDEFINED,
    XDG_SHELL,
    XDG_SHELL_V6,
    LAYER_SHELL
};

class MarinaView {
public:
    static void iterate_view_by_type(MarinaView* view, wlr_surface_iterator_func_t func, void* user_data);
    static MarinaView* view_at(MarinaServer* server, double lx, double ly, struct wlr_surface** surface, double* sx, double* sy);

    MarinaServer* server;

    int x = 0, y = 0;
	bool mapped = false;
    bool activated = false;

    int type = UNDEFINED;

    struct wl_list link;

public:
    bool is_view_at(double lx, double ly, struct wlr_surface** surface, double* sx, double* sy);

    void damage_whole();
    void activate();
};

#endif