/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-server.hpp"
#include "marina-input.hpp"
#include "marina-output.hpp"
#include "marina-xdg-view.hpp"
#include "marina-xdg-v6-view.hpp"
#include "marina-wl-view.hpp"

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

    this->export_dmabuf_manager_v1 = wlr_export_dmabuf_manager_v1_create(this->wl_display);

    this->idle = wlr_idle_create(this->wl_display);
    this->idle_inhibit = wlr_idle_inhibit_v1_create(this->wl_display);

    this->linux_dmabuf = wlr_linux_dmabuf_v1_create(this->wl_display, wlr_backend_get_renderer(this->backend));

    this->primary_selection_device_manager = wlr_gtk_primary_selection_device_manager_create(this->wl_display);

    this->compositor = wlr_compositor_create(this->wl_display, wlr_backend_get_renderer(this->backend));

    this->xdg_shell = wlr_xdg_shell_create(this->wl_display);

    this->new_xdg_surface.notify = new_xdg_surface_notify;
    wl_signal_add(&this->xdg_shell->events.new_surface, &this->new_xdg_surface);

    this->xdg_v6_shell = wlr_xdg_shell_v6_create(this->wl_display);

    this->new_xdg_v6_surface.notify = new_xdg_v6_surface_notify;
    wl_signal_add(&this->xdg_v6_shell->events.new_surface, &this->new_xdg_v6_surface);

    this->wl_shell = wlr_wl_shell_create(this->wl_display);

    this->new_wl_surface.notify = new_wl_surface_notify;
    wl_signal_add(&this->wl_shell->events.new_surface, &this->new_wl_surface);

    this->server_decoration_manager = wlr_server_decoration_manager_create(this->wl_display);
    wlr_server_decoration_manager_set_default_mode(this->server_decoration_manager, WLR_SERVER_DECORATION_MANAGER_MODE_SERVER);

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

void MarinaServer::new_wl_surface_notify(struct wl_listener *listener, void* data) {
    MarinaServer* server = wl_container_of(listener, server, new_xdg_surface);
    struct wlr_wl_shell_surface* wl_surface = (struct wlr_wl_shell_surface*) data;

    __attribute__((unused)) MarinaWLView* view = new MarinaWLView(server, wl_surface);
}

void MarinaServer::new_input_notify(struct wl_listener *listener, void* data) {
    struct wlr_input_device* device = (struct wlr_input_device*) data;
    MarinaServer* server = wl_container_of(listener, server, new_input);

    __attribute__((unused)) MarinaInput* input = new MarinaInput(server, device);
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

    this->output_layout = wlr_output_layout_create();

    this->seats[default_seat_name] = new MarinaSeat(this, default_seat_name);

    this->cursor = new MarinaCursor(this, this->output_layout);
    
    this->new_output.notify = MarinaServer::new_output_notify;
    wl_signal_add(&this->backend->events.new_output, &this->new_output);

    this->new_input.notify = MarinaServer::new_input_notify;
    wl_signal_add(&this->backend->events.new_input, &this->new_input);

    this->socket = wl_display_add_socket_auto(this->wl_display);
    assert(this->socket);
}

MarinaServer::~MarinaServer() {
    wlr_server_decoration_manager_destroy(this->server_decoration_manager);
    wlr_gtk_primary_selection_device_manager_destroy(this->primary_selection_device_manager);
    wlr_idle_destroy(this->idle);
    wlr_idle_inhibit_v1_destroy(this->idle_inhibit);
    wlr_backend_destroy(this->backend);
    wl_display_destroy(this->wl_display);
}