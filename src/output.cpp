/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-server.hpp"
#include "marina-output.hpp"
#include "marina-xdg-view.hpp"
#include "marina-xdg-v6-view.hpp"

MarinaOutput::MarinaOutput(struct wlr_output* wlr_output, MarinaServer* server) {
    clock_gettime(CLOCK_MONOTONIC, &this->last_frame);
    this->server = server;
    this->wlr_output = wlr_output;
    wl_list_insert(&server->outputs, &this->link);

    this->destroy.notify = destroy_notify;
    wl_signal_add(&wlr_output->events.destroy, &this->destroy);

    this->frame.notify = frame_notify;
    wl_signal_add(&wlr_output->events.frame, &this->frame);

    wlr_output_layout_add_auto(server->output_layout, wlr_output);

    wlr_output_create_global(wlr_output);

    this->renderer = wlr_backend_get_renderer(wlr_output->backend);
}

MarinaOutput::~MarinaOutput() {
    wl_list_remove(&this->link);
    wl_list_remove(&this->destroy.link);
    wl_list_remove(&this->frame.link);
}

void MarinaOutput::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaOutput* output = wl_container_of(listener, output, destroy);
    delete output;
}

void MarinaOutput::frame_notify(struct wl_listener* listener, void* data) {
    MarinaOutput* output = wl_container_of(listener, output, frame);
    MarinaServer* server = output->server;
    struct wlr_output* wlr_output = (struct wlr_output*) data;

    struct wlr_renderer* renderer = output->renderer;
    assert(renderer);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

// Beginning of GL Context
    wlr_output_make_current(wlr_output, NULL);

    int width, height;
    wlr_output_effective_resolution(wlr_output, &width, &height);
    wlr_renderer_begin(renderer, width, height);

    float clear_color[4] = {0.3, 0.4, 0.4, 1.0};
    wlr_renderer_clear(renderer, clear_color);

    MarinaView* view;
    wl_list_for_each_reverse(view, &server->views, link) {
        MarinaRenderer rdata;
        rdata.output   = wlr_output;
        rdata.view     = view;
        rdata.renderer = renderer;
        rdata.when     = &now;

        if (view->type == XDG_SHELL) {
            wlr_xdg_surface_for_each_surface(((MarinaXDGView*)view)->xdg_surface, MarinaRenderer::render_surface, &rdata);
        } else if (view->type == XDG_SHELL_V6) {
            wlr_xdg_surface_v6_for_each_surface(((MarinaXDGV6View*)view)->xdg_surface, MarinaRenderer::render_surface, &rdata);
        } else {
            wlr_log(WLR_ERROR, "View 0x%x with undefined type!", view);
        }
    }

    wlr_renderer_end(renderer);
    wlr_output_swap_buffers(wlr_output, NULL, NULL);
// End of GL Context
}