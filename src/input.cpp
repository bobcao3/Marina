/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-input.hpp"

void MarinaInput::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaInput* input = wl_container_of(listener, input, destroy);

    if (input->device->type) {
        input->server->cursor->detach_pointer(input);
    }

    delete input;
}

void MarinaInput::keyboard_key_notify(struct wl_listener* listener, void* data) {
	struct wlr_event_keyboard_key* event = (struct wlr_event_keyboard_key*) data;
    MarinaInput* input = wl_container_of(listener, input, keyboard_key);
	MarinaServer* server = input->server;

	uint32_t keycode = event->keycode + 8;

    for (const auto& p : server->seats) {
        MarinaSeat* seat = p.second;
        if (seat->keyboards.find(input) != seat->keyboards.end()) {
            seat->process_keyboard_key(event->time_msec, event->keycode, event->state);
        }
    }
}

void MarinaInput::keyboard_modifiers_notify(struct wl_listener* listener, void* data) {
    MarinaInput* input = wl_container_of(listener, input, keyboard_modifiers);
	MarinaServer* server = input->server;
    
    for (const auto& p : server->seats) {
        MarinaSeat* seat = p.second;
        if (seat->keyboards.find(input) != seat->keyboards.end()) {
            seat->process_keyboard_modifiers(&input->device->keyboard->modifiers);
        }
    }
}

void xkb_init(struct wlr_input_device* device) {
    struct xkb_rule_names rules = {0};
    rules.rules = getenv("XKB_DEFAULT_RULES");
    rules.model = getenv("XKB_DEFAULT_MODEL");
    rules.layout = getenv("XKB_DEFAULT_LAYOUT");
    rules.variant = getenv("XKB_DEFAULT_VARIANT");
    rules.options = getenv("XKB_DEFAULT_OPTIONS");
    struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!context) {
        wlr_log(WLR_ERROR, "Failed to create XKB context");
        exit(1);
    }
    wlr_keyboard_set_keymap(device->keyboard, xkb_map_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS));
    xkb_context_unref(context);
}

MarinaInput::MarinaInput(MarinaServer* server, struct wlr_input_device* device) {
    this->server = server;
    this->device = device;

    this->destroy.notify = MarinaInput::destroy_notify;
    wl_signal_add(&device->events.destroy, &this->destroy);

    MarinaSeat* default_seat = server->seats[default_seat_name];
    assert(default_seat);

    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
        wlr_log(WLR_DEBUG, "New keyboard input device %x.", this);

        this->keyboard_key.notify = MarinaInput::keyboard_key_notify;
        wl_signal_add(&device->keyboard->events.key, &this->keyboard_key);

        this->keyboard_modifiers.notify = MarinaInput::keyboard_modifiers_notify;
        wl_signal_add(&device->keyboard->events.modifiers, &this->keyboard_modifiers);

        xkb_init(device);

        default_seat->add_keyboard(this);

        break;
    case WLR_INPUT_DEVICE_POINTER:
        default_seat->add_pointer(this);

    case WLR_INPUT_DEVICE_TOUCH:
    case WLR_INPUT_DEVICE_TABLET_TOOL:
        server->cursor->bind_pointer(this);
        break;
    case WLR_INPUT_DEVICE_TABLET_PAD:
    default:
        break;
    }
}

MarinaInput::~MarinaInput() {
    wlr_log(WLR_DEBUG, "Destroy input device %x.", this);    
}