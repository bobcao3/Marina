/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#include "marina-view.hpp"
#include "marina-output.hpp"

void MarinaView::damage_whole() {
    MarinaOutput* output;
    wl_list_for_each(output, &this->server->outputs, link) {
        output->damage_whole();
    }
}

void MarinaView::activate() {
    this->activated = true;
}