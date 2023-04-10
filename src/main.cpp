#include <iostream>

#include "event2/event.h"
#include "event2/listener.h"

#include "auto_reserve.hpp"
#include "web_server.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Please Input Port!" << std::endl;
    }

    event_base* base;

#ifdef WIN32s
    WSADATA wsa_data;
    WSAStartup(0x0201, &wsa_data);
#endif

    base = event_base_new();
    if(!base) {
        std::cerr << "Init libevent base failed!" << std::endl;
        exit(1);
    }

    WebServer web_srv(atoi(argv[1]));
}