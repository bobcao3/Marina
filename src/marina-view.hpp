/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */


#ifndef __INCLUDE_MARINA_VIEW_HPP
#define __INCLUDE_MARINA_VIEW_HPP

#define WLR_USE_UNSTABLE

#include "marina-server.hpp"

class MarinaView {
public:
    MarinaServer* server;

    int x = 0, y = 0;
	bool mapped = false;

    struct wl_list link;
};

#endif