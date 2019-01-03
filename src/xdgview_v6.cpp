/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-xdg-v6-view.hpp"

MarinaXDGV6View::MarinaXDGV6View(MarinaServer* server, wlr_xdg_surface_v6* xdg_surface) {
    this->server = server;
    this->xdg_surface = xdg_surface;

    this->type = XDG_SHELL_V6;

    wlr_log(WLR_DEBUG, "new MarinaXDGV6View %x\n", this);

    this->map.notify = map_notify;
    wl_signal_add(&xdg_surface->events.map, &this->map);

    this->unmap.notify = unmap_notify;
    wl_signal_add(&xdg_surface->events.unmap, &this->unmap);

    this->destroy.notify = destroy_notify;
    wl_signal_add(&xdg_surface->events.destroy, &this->destroy);

    struct wlr_xdg_toplevel_v6 *toplevel = xdg_surface->toplevel;

    this->request_move.notify = request_move_notify;
    wl_signal_add(&toplevel->events.request_move, &this->request_move);

    this->request_resize.notify = request_resize_notify;
    wl_signal_add(&toplevel->events.request_resize, &this->request_resize);

    server->views.push_back(this);
}

void MarinaXDGV6View::map_notify(struct wl_listener* listener, void* data) {
    MarinaXDGV6View* view = wl_container_of(listener, view, map);
    view->mapped = true;
    view->activate();

    view->server->seats[default_seat_name]->focus_view(view, view->xdg_surface->surface);    

    view->damage_whole();
}

void MarinaXDGV6View::unmap_notify(struct wl_listener* listener, void* data) {
    MarinaXDGV6View* view = wl_container_of(listener, view, unmap);
    view->mapped = false;

    view->damage_whole();
}

void MarinaXDGV6View::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaXDGV6View* view = wl_container_of(listener, view, destroy);
    view->server->views.remove(view);
    wlr_log(WLR_DEBUG, "MarinaXDGV6View %x destroyed", view);
    delete view;
}

void MarinaXDGV6View::request_move_notify(struct wl_listener* listener, void* data) {
    struct wlr_xdg_toplevel_resize_event* event = (struct wlr_xdg_toplevel_resize_event*) data;
    MarinaXDGV6View* view = wl_container_of(listener, view, request_move);

}

void MarinaXDGV6View::request_resize_notify(struct wl_listener* listener, void* data) {
    struct wlr_xdg_toplevel_resize_event* event = (struct wlr_xdg_toplevel_resize_event*) data;
    MarinaXDGV6View* view = wl_container_of(listener, view, request_resize);

}

void MarinaXDGV6View::activate() {
    this->activated = true;
    if (this->xdg_surface->role == WLR_XDG_SURFACE_V6_ROLE_TOPLEVEL) {
        wlr_xdg_toplevel_v6_set_activated(this->xdg_surface, activated);
    }
}