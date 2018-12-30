/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */

#include "marina-server.hpp"

int main(int argc, char **argv) {
    enum wlr_log_importance log_importance = WLR_DEBUG;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v")) {
            log_importance = WLR_DEBUG;
        }
    }

    wlr_log_init(log_importance, NULL);

    MarinaServer* server = new MarinaServer();

    int ret = server->run_server();

    delete server;
    return ret;
}