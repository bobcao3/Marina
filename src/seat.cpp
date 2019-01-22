/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-seat.hpp"

MarinaSeat::MarinaSeat(MarinaServer* server, const char* name) {
    this->server = server;
    this->seat = wlr_seat_create(server->wl_display, name);
}

MarinaSeat::~MarinaSeat() {
    wlr_seat_destroy(this->seat);
}

void MarinaSeat::add_keyboard(MarinaInput* input) {
    wlr_seat_set_keyboard(this->seat, input->device);
    this->keyboards.insert(input);
    this->update_capabilities();
}

void MarinaSeat::add_pointer(MarinaInput* input) {
    this->pointers.insert(input);
    this->update_capabilities();
}

void MarinaSeat::add_touch(MarinaInput* input) {
    this->touches.insert(input);
    this->update_capabilities();
}

void MarinaSeat::update_capabilities() {
    uint32_t capabilities = 0x0;

    if (!this->keyboards.empty()) {
        capabilities |= WL_SEAT_CAPABILITY_KEYBOARD;
    }

    if (!this->pointers.empty()) {
        capabilities |= WL_SEAT_CAPABILITY_POINTER;
    }

    if (!this->touches.empty()) {
        capabilities |= WL_SEAT_CAPABILITY_TOUCH;
    }

    wlr_seat_set_capabilities(this->seat, capabilities);

    // TODO： Hide cursor when there isn't one
}

void MarinaSeat::process_cursor_motion(struct wlr_cursor* wlr_cursor, uint32_t time) {
    double sx, sy;
    struct wlr_surface* surface = NULL;
    MarinaView* view = MarinaView::view_at(this->server, wlr_cursor->x, wlr_cursor->y, &surface, &sx, &sy);

    wlr_idle_notify_activity(this->server->idle, this->seat);

    if (surface) {
        wlr_seat_pointer_notify_enter(this->seat, surface, sx, sy);
        if (time > 0) {
            wlr_seat_pointer_notify_motion(this->seat, time, sx, sy);        
        }
    } else {
        wlr_seat_pointer_clear_focus(this->seat);
    }
}

void MarinaSeat::process_cursor_button(struct wlr_cursor* wlr_cursor, uint32_t time, uint32_t button, uint32_t state) {
    double sx, sy;
    struct wlr_surface* surface = NULL;
    MarinaView* view = MarinaView::view_at(this->server, wlr_cursor->x, wlr_cursor->y, &surface, &sx, &sy);

    wlr_idle_notify_activity(this->server->idle, this->seat);

    if (view != this->active_view || surface != this->active_surface) focus_view(view, surface);

    if (time > 0) wlr_seat_pointer_send_button(this->seat, time, button, state);
}

void MarinaSeat::process_keyboard_key(uint32_t time, uint32_t keycode, uint32_t state) {
    wlr_idle_notify_activity(this->server->idle, this->seat);

    if (time > 0) {
        wlr_seat_keyboard_send_key(this->seat, time, keycode, state);
    }
}

void MarinaSeat::focus_view(MarinaView* view, struct wlr_surface* surface) {
    wlr_log(WLR_DEBUG, "Focus view %x", view);

    if (this->active_view) {
        this->active_view->deactivate();
    }

    this->active_view = view;
    this->active_surface = surface;

    if (view && surface) {
        view->activate();

        struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(this->seat);
        if (keyboard) {
            wlr_seat_keyboard_notify_enter(this->seat, surface, keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
        } else {
            wlr_seat_keyboard_notify_enter(this->seat, surface, NULL, 0, NULL);
        }
    }
}