/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-xdg-view.hpp"

MarinaXDGView::MarinaXDGView(MarinaServer* server, wlr_xdg_surface* xdg_surface) {
    this->server = server;
    this->xdg_surface = xdg_surface;

    this->type = XDG_SHELL;

    wlr_log(WLR_DEBUG, "new MarinaXDGView %x\n", this);

    this->map.notify = map_notify;
    wl_signal_add(&xdg_surface->events.map, &this->map);

    this->unmap.notify = unmap_notify;
    wl_signal_add(&xdg_surface->events.unmap, &this->unmap);

    this->destroy.notify = destroy_notify;
    wl_signal_add(&xdg_surface->events.destroy, &this->destroy);

    struct wlr_xdg_toplevel *toplevel = xdg_surface->toplevel;

    this->request_move.notify = request_move_notify;
    wl_signal_add(&toplevel->events.request_move, &this->request_move);

    this->request_resize.notify = request_resize_notify;
    wl_signal_add(&toplevel->events.request_resize, &this->request_resize);

    server->views.push_back(this);
}

void MarinaXDGView::map_notify(struct wl_listener* listener, void* data) {
    MarinaXDGView* view = wl_container_of(listener, view, map);
    view->mapped = true;

    view->server->seats[default_seat_name]->focus_view(view, view->xdg_surface->surface);

    view->damage_whole();
}

void MarinaXDGView::unmap_notify(struct wl_listener* listener, void* data) {
    MarinaXDGView* view = wl_container_of(listener, view, unmap);
    view->mapped = false;

    view->damage_whole();
}

void MarinaXDGView::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaXDGView* view = wl_container_of(listener, view, destroy);
    view->server->views.remove(view);
    delete view;
}

void MarinaXDGView::request_move_notify(struct wl_listener* listener, void* data) {
    struct wlr_xdg_toplevel_resize_event* event = (struct wlr_xdg_toplevel_resize_event*) data;
    MarinaXDGView* view = wl_container_of(listener, view, request_move);

}

void MarinaXDGView::request_resize_notify(struct wl_listener* listener, void* data) {
    struct wlr_xdg_toplevel_resize_event* event = (struct wlr_xdg_toplevel_resize_event*) data;
    MarinaXDGView* view = wl_container_of(listener, view, request_resize);

}

void MarinaXDGView::activate() {
    this->activated = true;
    if (this->xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
        wlr_xdg_toplevel_set_activated(this->xdg_surface, activated);
    }
}