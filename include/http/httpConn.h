#pragma once
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/buffer.h"

#include "http/httpRequest.h"
#include "http/httpResponse.h"

#include <arpa/inet.h>

class HttpConn {
    int fd_;
    bufferevent* bev_;
    char ip_[16];

    HttpRequest request_;
    HttpResponse response_;

public:
    HttpConn() = default;
    void init(int fd, bufferevent* ev, char* ip);
    void processReq();
    int getFd();
    char* getIP();

    static const char* src_dir_;
    static int user_cnt_;

};