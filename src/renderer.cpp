/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-renderer.hpp"

void MarinaRenderer::render_surface(struct wlr_surface* surface, int sx, int sy, void* data) {
    MarinaRenderer*    renderer = (MarinaRenderer*) data;
    MarinaView*        view     = renderer->view;
    struct wlr_output* output = renderer->output;

    if (!view->mapped) { return; }

    struct wlr_texture *texture = wlr_surface_get_texture(surface);
    if (!texture) { return; }

    double ox = 0, oy = 0;
    wlr_output_layout_output_coords(view->server->output_layout, output, &ox, &oy);
    ox += view->x + sx, oy += view->y + sy;

    struct wlr_box box = {
        .x = (int) (ox * output->scale),
        .y = (int) (oy * output->scale),
        .width = (int) (surface->current.width * output->scale),
        .height = (int) (surface->current.height * output->scale),
    };

    float matrix[9];
    enum wl_output_transform transform = wlr_output_transform_invert(surface->current.transform);
    wlr_matrix_project_box(matrix, &box, transform, 0, output->transform_matrix);

    wlr_render_texture_with_matrix(renderer->renderer, texture, matrix, 1.0);

    wlr_surface_send_frame_done(surface, renderer->when);
}