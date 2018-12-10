/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-xdg-view.hpp"

MarinaXDGView::MarinaXDGView(MarinaServer* server, wlr_xdg_surface* xdg_surface) {
    this->server = server;
    this->xdg_surface = xdg_surface;

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

    wl_list_insert(&server->views, &this->link);
}

void MarinaXDGView::map_notify(struct wl_listener* listener, void* data) {
    MarinaXDGView* view = wl_container_of(listener, view, map);
    view->mapped = true;
    // TODO : Focus this view
}

void MarinaXDGView::unmap_notify(struct wl_listener* listener, void* data) {
    MarinaXDGView* view = wl_container_of(listener, view, unmap);
    view->mapped = false;
}

void MarinaXDGView::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaXDGView* view = wl_container_of(listener, view, destroy);
    wl_list_remove(&view->link);
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