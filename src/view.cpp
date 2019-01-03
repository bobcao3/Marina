/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-view.hpp"
#include "marina-output.hpp"
#include "marina-xdg-v6-view.hpp"
#include "marina-xdg-view.hpp"

void MarinaView::damage_whole() {
    for (MarinaOutput* output : server->outputs) {
        output->damage_whole();
    }
}

void MarinaView::activate() {
    this->activated = true;

    this->server->views.remove(this);
    this->server->views.push_back(this);
}

void MarinaView::deactivate() {
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

bool MarinaView::is_view_at(double lx, double ly, struct wlr_surface** surface, double* sx, double* sy) {
    double view_sx = lx - this->x;
    double view_sy = ly - this->y;

    double _sx, _sy;
    struct wlr_surface *_surface = NULL;
    if (this->type == XDG_SHELL) {
        _surface = wlr_xdg_surface_surface_at(((MarinaXDGView*)this)->xdg_surface, view_sx, view_sy, &_sx, &_sy);
    } else if (this->type == XDG_SHELL_V6) {
        _surface = wlr_xdg_surface_v6_surface_at(((MarinaXDGV6View*)this)->xdg_surface, view_sx, view_sy, &_sx, &_sy);
    } else {
        return false;
    }

    if (_surface != NULL) {
        *sx = _sx;
        *sy = _sy;
        *surface = _surface;
        return true;
    }

    return false;
}

MarinaView* MarinaView::view_at(MarinaServer* server, double lx, double ly, struct wlr_surface** surface, double* sx, double* sy) {
    for (std::list<MarinaView*>::reverse_iterator rit=server->views.rbegin(); rit!=server->views.rend(); ++rit) {
        MarinaView* view = *rit;
        if (view->is_view_at(lx, ly, surface, sx, sy)) {
            return view;
        }
    }
    return NULL;
}