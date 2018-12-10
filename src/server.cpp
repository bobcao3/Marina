/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-server.hpp"
#include "marina-output.hpp"
#include "marina-xdg-view.hpp"
#include "marina-xdg-v6-view.hpp"

int MarinaServer::run_server() {
    if (!wlr_backend_start(this->backend)) {
        wlr_log(WLR_ERROR, "Failed to start backend.\n");
        return 1;
    }

    wlr_log(WLR_INFO, "Running compositor on wayland display '%s'.\n", this->socket);
    setenv("_WAYLAND_DISPLAY", this->socket, true);
    setenv("WAYLAND_DISPLAY", this->socket, true);

    wl_display_init_shm(this->wl_display);
    wlr_gamma_control_manager_create(this->wl_display);
    wlr_screenshooter_create(this->wl_display);
    wlr_idle_create(this->wl_display);

    this->compositor = wlr_compositor_create(this->wl_display, wlr_backend_get_renderer(this->backend));

    this->xdg_shell = wlr_xdg_shell_create(this->wl_display);

    this->new_xdg_surface.notify = new_xdg_surface_notify;
    wl_signal_add(&this->xdg_shell->events.new_surface, &this->new_xdg_surface);

    this->xdg_v6_shell = wlr_xdg_shell_v6_create(this->wl_display);

    this->new_xdg_v6_surface.notify = new_xdg_v6_surface_notify;
    wl_signal_add(&this->xdg_v6_shell->events.new_surface, &this->new_xdg_v6_surface);

    wl_display_run(this->wl_display);
    return 0;
}

void MarinaServer::new_xdg_v6_surface_notify(struct wl_listener *listener, void* data) {
    MarinaServer* server = wl_container_of(listener, server, new_xdg_v6_surface);
    struct wlr_xdg_surface_v6* xdg_surface = (struct wlr_xdg_surface_v6*) data;

    if (xdg_surface->role != WLR_XDG_SURFACE_V6_ROLE_TOPLEVEL) { return; }
    
    __attribute__((unused)) MarinaXDGV6View* view = new MarinaXDGV6View(server, xdg_surface);
}

void MarinaServer::new_xdg_surface_notify(struct wl_listener *listener, void* data) {
    MarinaServer* server = wl_container_of(listener, server, new_xdg_surface);
    struct wlr_xdg_surface* xdg_surface = (struct wlr_xdg_surface*) data;

    if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) { return; }
    
    __attribute__((unused)) MarinaXDGView* view = new MarinaXDGView(server, xdg_surface);
}

void MarinaServer::new_output_notify(struct wl_listener *listener, void* data) {
    MarinaServer* server = wl_container_of(listener, server, new_output);
    struct wlr_output* wlr_output = (struct wlr_output*) data;

    if (!wl_list_empty(&wlr_output->modes)) {
        struct wlr_output_mode* mode = wl_container_of(wlr_output->modes.prev, mode, link);
        wlr_output_set_mode(wlr_output, mode);
    }

    __attribute__((unused)) MarinaOutput* output = new MarinaOutput(wlr_output, server);
}

MarinaServer::MarinaServer() {
    // Create Wayland Display
    this->wl_display = wl_display_create();
    assert(this->wl_display);
    this->wl_event_loop = wl_display_get_event_loop(this->wl_display);
    assert(this->wl_event_loop);

    // Create wlroots backend
    this->backend = wlr_backend_autocreate(this->wl_display, NULL);
    assert(this->backend);

    wl_list_init(&this->outputs);
    wl_list_init(&this->views);

    this->output_layout = wlr_output_layout_create();
    
    this->new_output.notify = MarinaServer::new_output_notify;
    wl_signal_add(&this->backend->events.new_output, &this->new_output);

    this->socket = wl_display_add_socket_auto(this->wl_display);
    assert(this->socket);
}

MarinaServer::~MarinaServer() {
    wlr_backend_destroy(this->backend);
    wl_display_destroy(this->wl_display);
}