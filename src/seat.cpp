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
    this->keyboards.push_back(input);
    this->update_capabilities();
}

void MarinaSeat::add_pointer(MarinaInput* input) {
    this->pointers.push_back(input);
    this->update_capabilities();
}

void MarinaSeat::add_touch(MarinaInput* input) {
    this->touches.push_back(input);
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