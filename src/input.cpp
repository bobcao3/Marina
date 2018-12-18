/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-input.hpp"

void MarinaInput::destroy_notify(struct wl_listener* listener, void* data) {
    MarinaInput* input = wl_container_of(listener, input, destroy);
    delete input;
}

void MarinaInput::keyboard_key_notify(struct wl_listener* listener, void* data) {

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

    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:;
        wlr_log(WLR_DEBUG, "New keyboard input device %x.", this);

        this->destroy.notify = MarinaInput::destroy_notify;
        wl_signal_add(&device->events.destroy, &this->destroy);

        this->keyboard_key.notify = MarinaInput::keyboard_key_notify;
        wl_signal_add(&device->keyboard->events.key, &this->keyboard_key);

        xkb_init(device);

        break;
    case WLR_INPUT_DEVICE_POINTER:
    case WLR_INPUT_DEVICE_TOUCH:
    case WLR_INPUT_DEVICE_TABLET_TOOL:
    case WLR_INPUT_DEVICE_TABLET_PAD:
        break;
    }
}

MarinaInput::~MarinaInput() {
    wlr_log(WLR_DEBUG, "Destroy input device %x.", this);    
}