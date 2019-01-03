/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-input.hpp"
#include "marina-view.hpp"

const char* cursor_theme_name = "Adwaita";
const int cursor_size = 16;

void MarinaCursor::bind_pointer(MarinaInput* input) {
    wlr_log(WLR_DEBUG, "Pointer device %x binded", input);
    wlr_cursor_attach_input_device(this->wlr_cursor, input->device);
}

void MarinaCursor::detach_pointer(MarinaInput* input) {
    wlr_log(WLR_DEBUG, "Pointer device %x detached", input);
    wlr_cursor_detach_input_device(this->wlr_cursor, input->device);
}

void MarinaCursor::motion_notify(struct wl_listener* listener, void* data) {
    MarinaCursor* cursor = wl_container_of(listener, cursor, motion);
    struct wlr_event_pointer_motion* event = (struct wlr_event_pointer_motion*) data;
    wlr_cursor_move(cursor->wlr_cursor, event->device, event->delta_x, event->delta_y);

    for (const auto& p : cursor->server->seats) {
        p.second->process_cursor_motion(cursor->wlr_cursor, event->time_msec);
    }
}

void MarinaCursor::motion_absolute_notify(struct wl_listener* listener, void* data) {
    MarinaCursor* cursor = wl_container_of(listener, cursor, motion_absolute);
    struct wlr_event_pointer_motion_absolute* event = (struct wlr_event_pointer_motion_absolute*) data;
    wlr_cursor_warp_absolute(cursor->wlr_cursor, event->device, event->x, event->y);

    for (const auto& p : cursor->server->seats) {
        p.second->process_cursor_motion(cursor->wlr_cursor, event->time_msec);
    }
}

void MarinaCursor::button_notify(struct wl_listener* listener, void* data) {
    MarinaCursor* cursor = wl_container_of(listener, cursor, button);
    struct wlr_event_pointer_button* event = (struct wlr_event_pointer_button*) data;

    for (const auto& p : cursor->server->seats) {
        p.second->process_cursor_button(cursor->wlr_cursor, event->time_msec, event->button, event->state);
    }
}

MarinaCursor::MarinaCursor(MarinaServer* server, struct wlr_output_layout* layout) {
    this->server = server;
    this->wlr_cursor = wlr_cursor_create();
    wlr_cursor_attach_output_layout(this->wlr_cursor, layout);

    wl_signal_add(&this->wlr_cursor->events.motion, &this->motion);
    this->motion.notify = MarinaCursor::motion_notify;

    wl_signal_add(&this->wlr_cursor->events.motion_absolute, &this->motion_absolute);
    this->motion_absolute.notify = MarinaCursor::motion_absolute_notify;

    wl_signal_add(&this->wlr_cursor->events.button, &this->button);
    this->button.notify = MarinaCursor::button_notify;

    this->xcursor_manager = wlr_xcursor_manager_create("default", 24);
	if (!this->xcursor_manager) {
		wlr_log(WLR_ERROR, "Failed to load left_ptr cursor");
		exit(1);
	}
    wlr_xcursor_manager_set_cursor_image(this->xcursor_manager, "left_ptr", this->wlr_cursor);
}

MarinaCursor::~MarinaCursor() {
    wlr_xcursor_manager_destroy(this->xcursor_manager);
    wlr_cursor_destroy(this->wlr_cursor);
}