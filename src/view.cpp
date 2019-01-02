/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-view.hpp"
#include "marina-output.hpp"
#include "marina-xdg-v6-view.hpp"
#include "marina-xdg-view.hpp"

void MarinaView::damage_whole() {
    MarinaOutput* output;
    wl_list_for_each(output, &this->server->outputs, link) {
        output->damage_whole();
    }
}

void MarinaView::activate() {
    this->activated = true;
}

void MarinaView::iterate_view_by_type(MarinaView* view, wlr_surface_iterator_func_t func, void* user_data) {
    if (view->type == XDG_SHELL) {
        wlr_xdg_surface_for_each_surface(((MarinaXDGView*)view)->xdg_surface, func, user_data);
    } else if (view->type == XDG_SHELL_V6) {
        wlr_xdg_surface_v6_for_each_surface(((MarinaXDGV6View*)view)->xdg_surface, func, user_data);
    } else {
        wlr_log(WLR_ERROR, "View 0x%x with undefined type!", view);
    }
}