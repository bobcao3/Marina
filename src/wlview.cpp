/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-wl-view.hpp"

MarinaWLView::MarinaWLView(MarinaServer* server, wlr_wl_shell_surface* wl_shell_surface) {
    this->server = server;
    this->wl_shell_surface = wl_shell_surface;

    this->type = XDG_SHELL;

    wlr_log(WLR_DEBUG, "new MarinaWLView %x\n", this);

    this->destroy.notify = destroy_notify;
    wl_signal_add(&wl_shell_surface->events.destroy, &this->destroy);

    this->request_move.notify = request_move_notify;
    wl_signal_add(&wl_shell_surface->events.request_move, &this->request_move);

    this->request_resize.notify = request_resize_notify;
    wl_signal_add(&wl_shell_surface->events.request_resize, &this->request_resize);

    server->views.push_back(this);

    this->mapped = true;
    this->server->seats[default_seat_name]->focus_view(this, this->wl_shell_surface->surface);

    this->damage_whole();
}

void MarinaWLView::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaWLView* view = wl_container_of(listener, view, destroy);
    view->server->views.remove(view);
    delete view;
}

void MarinaWLView::request_move_notify(struct wl_listener* listener, void* data) {
    struct wlr_wl_shell_surface_move_event* event = (struct wlr_wl_shell_surface_move_event*) data;
    MarinaWLView* view = wl_container_of(listener, view, request_move);

}

void MarinaWLView::request_resize_notify(struct wl_listener* listener, void* data) {
    struct wlr_wl_shell_surface_resize_event* event = (struct wlr_wl_shell_surface_resize_event*) data;
    MarinaWLView* view = wl_container_of(listener, view, request_resize);

}