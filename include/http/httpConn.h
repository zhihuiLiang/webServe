#pragma once
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/buffer.h"

#include "httpRequest.h"

#include <arpa/inet.h>

class HttpConn {
    int fd_;
    bufferevent* bev_;
    sockaddr_in addr_;

    HttpRequest request_;
public:
    HttpConn() = default;
    void init(int fd, bufferevent* ev, sockaddr_in* addr);
    void processReq(evbuffer* ev);
    int getFd();
    ~HttpConn();
};