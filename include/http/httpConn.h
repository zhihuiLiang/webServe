#pragma once
#include <arpa/inet.h>

class httpConn {
    int fd_;
    sockaddr_in addr_;

public:
    httpConn() = default;
    void init(int fd, sockaddr_in* addr);
};