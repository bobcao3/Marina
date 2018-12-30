/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-renderer.hpp"
#include "marina-output.hpp"

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

    pixman_region32_t damage;
    pixman_box32_t* rects;

    pixman_region32_init(&damage);
    pixman_region32_union_rect(&damage, &damage, box.x, box.y, box.width, box.height);
    pixman_region32_intersect(&damage, &damage, renderer->damage);
    bool damaged = pixman_region32_not_empty(&damage);
    if (!damaged) {
        goto damage_finish;
    }

    int nrects;
    rects = pixman_region32_rectangles(&damage, &nrects);
    for (int i = 0; i < nrects; ++i) {
        MarinaOutput::scissor_output(output, renderer->renderer, &rects[i]);
        wlr_render_texture_with_matrix(renderer->renderer, texture, matrix, 1.0);
    }

damage_finish:
    wlr_surface_send_frame_done(surface, renderer->when);
}