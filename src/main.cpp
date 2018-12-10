/* 
 * Copyright (c) 2018, Cheng 'Bob' Cao
 * All rights reserved.
 */

#include "marina-server.hpp"

int main(int argc, char **argv) {
    wlr_log_init(WLR_DEBUG, NULL);

    MarinaServer* server = new MarinaServer();

    int ret = server->run_server();

    delete server;
    return ret;
}