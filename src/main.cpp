#include <iostream>

#include "event2/event.h"
#include "event2/listener.h"

#include "auto_reserve.hpp"
#include "web_server.h"

int main(int argc, char** argv) {
    event_base* base;

    base = event_base_new();
    if(!base) {
        std::cerr << "Init libevent base failed!" << std::endl;
        exit(1);
    }

    WebServer web_srv;
    AutoReserve auto_rsv;
    auto_rsv.initWebSrvPtr(&web_srv);
    auto_rsv.reserve();
    event_base_dispatch(base);

    event_base_free(base);
}