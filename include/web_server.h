#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/listener.h"

#include <iostream>

class WebServer {
    sockaddr_in addr_;
    event_base* base_;
    evconnlistener* listener_;

private:
    static void listenCB(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socklen, void* usr_data);
    static void readCB(bufferevent* bev, void* ctx);
    static void beventCB(bufferevent* bev, short what, void* ctx);

public:
    WebServer(int port);
    void setEventBase(event_base* base);
    void initListenerAndBind();
    bufferevent* connectSrv(const sockaddr_in addr);
};