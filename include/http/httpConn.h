#pragma once
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/buffer.h"

#include "http/httpRequest.h"
#include "http/httpResponse.h"

#include <arpa/inet.h>

#include <atomic>

class HttpConn {
    int fd_;
    bufferevent* bev_;
    char ip_[16];

    HttpRequest request_;
    HttpResponse response_;

public:
    HttpConn() = default;
    void init(int fd, bufferevent* ev, const char* ip);
    void processReq();
    void sendReq(std::string method, std::string path, const std::string& header, const std::string& body);
    int getFd();
    char* getIP();

    static const char* src_dir_;
    static int user_cnt_;

    std::atomic<bool> task_finish;

};