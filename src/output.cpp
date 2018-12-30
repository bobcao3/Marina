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
    this->damage = wlr_output_damage_create(wlr_output);
    wl_list_insert(&server->outputs, &this->link);

    this->destroy.notify = destroy_notify;
    wl_signal_add(&wlr_output->events.destroy, &this->destroy);

    this->damage_frame.notify = damage_frame_notify;
    wl_signal_add(&this->damage->events.frame, &this->damage_frame);

    this->damage_destroy.notify = damage_destroy_notify;
    wl_signal_add(&this->damage->events.destroy, &this->damage_destroy);

    wlr_output_layout_add_auto(server->output_layout, wlr_output);

    wlr_output_create_global(wlr_output);

    this->renderer = wlr_backend_get_renderer(wlr_output->backend);
    
    wlr_xcursor_manager_load(server->cursor->xcursor_manager, wlr_output->scale);
    wlr_xcursor_manager_set_cursor_image(server->cursor->xcursor_manager, "left_ptr", server->cursor->wlr_cursor);

    this->damage_whole();
}

MarinaOutput::~MarinaOutput() {
    wl_list_remove(&this->link);
    wl_list_remove(&this->destroy.link);
    wl_list_remove(&this->damage_destroy.link);
    wl_list_remove(&this->damage_frame.link);
}

void MarinaOutput::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaOutput* output = wl_container_of(listener, output, destroy);
    delete output;
}

void MarinaOutput::damage_destroy_notify(struct wl_listener *listener, void *data) {
    MarinaOutput* output = wl_container_of(listener, output, damage_destroy);
    delete output;
}

void MarinaOutput::scissor_output(struct wlr_output* wlr_output, struct wlr_renderer* renderer, pixman_box32_t *rect) {
    struct wlr_box box = {
        .x = rect->x1,
        .y = rect->y1,
        .width = rect->x2 - rect->x1,
        .height = rect->y2 - rect->y1,
    };

    int ow, oh;
    wlr_output_transformed_resolution(wlr_output, &ow, &oh);

    enum wl_output_transform transform = wlr_output_transform_invert(wlr_output->transform);
    wlr_box_transform(&box, transform, ow, oh, &box);

    wlr_renderer_scissor(renderer, &box);
}

void MarinaOutput::damage_whole() {
    wlr_output_damage_add_whole(this->damage);
    wlr_log(WLR_DEBUG, "Damage whole");
}

void MarinaOutput::render_output() {
    assert(this->renderer);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    const float clear_color[4] = {0.3f, 0.4f, 0.4f, 1.0f};

// Beginning of GL Context
    bool needs_swap;
    pixman_region32_t damage;
    pixman_box32_t *rects;
    pixman_region32_init(&damage);
    if (!wlr_output_damage_make_current(this->damage, &needs_swap, &damage)) {
        return;
    }

    if (!needs_swap) {
        // Output doesn't need swap and isn't damaged, skip rendering completely
        goto damage_finish;
    }

    if (!pixman_region32_not_empty(&damage)) {
        // Output isn't damaged but needs buffer swap
        goto renderer_end;
    }

    int width, height;
    wlr_output_effective_resolution(this->wlr_output, &width, &height);
    wlr_renderer_begin(renderer, width, height);

    int nrects;
    rects = pixman_region32_rectangles(&damage, &nrects);
    for (int i = 0; i < nrects; ++i) {
        scissor_output(wlr_output, this->renderer, &rects[i]);
        wlr_renderer_clear(this->renderer, clear_color);
    }

    MarinaView* view;
    wl_list_for_each_reverse(view, &server->views, link) {
        MarinaRenderer rdata;
        rdata.output   = this->wlr_output;
        rdata.view     = view;
        rdata.renderer = renderer;
        rdata.damage   = &damage;
        rdata.when     = &now;

        if (view->type == XDG_SHELL) {
            wlr_xdg_surface_for_each_surface(((MarinaXDGView*)view)->xdg_surface, MarinaRenderer::render_surface, &rdata);
        } else if (view->type == XDG_SHELL_V6) {
            wlr_xdg_surface_v6_for_each_surface(((MarinaXDGV6View*)view)->xdg_surface, MarinaRenderer::render_surface, &rdata);
        } else {
            wlr_log(WLR_ERROR, "View 0x%x with undefined type!", view);
        }
    }

renderer_end:
    wlr_output_render_software_cursors(this->wlr_output, &damage);
    wlr_renderer_scissor(renderer, NULL);
    wlr_output_swap_buffers(this->wlr_output, NULL, NULL);
    wlr_renderer_end(renderer);
// End of GL Context

damage_finish:
    pixman_region32_fini(&damage);
}

void MarinaOutput::damage_frame_notify(struct wl_listener* listener, void* data) {
    MarinaOutput* output = wl_container_of(listener, output, damage_frame);
    output->render_output();
}