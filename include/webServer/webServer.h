#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <event2/listener.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "http/httpConn.h"
#include "utility.h"

#include <unistd.h>
#include <unordered_map>

class WebServer {
    sockaddr_in addr_;
    event_base* base_;
    evconnlistener* listener_;

    char* src_dir_;
    std::unordered_map<int, HttpConn> users_;

public:
    WebServer(int port = 80);
    ~WebServer();

    bufferevent* connectSrv(std::string host, int port = 80);
    void sendHttpReq(bufferevent* bev, std::string method, std::string path, std::string header, std::string content);

    friend void listenCB(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socklen, void* usr_data);
    friend void readCB(bufferevent* bev, void* ctx);
    friend void beventCB(bufferevent* bev, short what, void* ctx);
};