#include "web_server.h"

WebServer::WebServer(int port) : base_(nullptr) {
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = INADDR_ANY;
}

void WebServer::setEventBase(event_base* base) {
    base_ = base;
}

void WebServer::initListenerAndBind() {
    listener_ = evconnlistener_new_bind(base_, listenCB, (void *)base_, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, reinterpret_cast<sockaddr*)(&addr_), sizeof(addr_));
    if(!listener_) {
        std::cerr << "Create lisener failed" << std::endl;
        exit(1);
    }
}

bufferevent* WebServer::connectSrv(const sockaddr_in* addr) {
    bufferevent* bev = bufferevent_socket_new(base_, -1, BEV_OPT_CLOSE_ON_FREE);
    int ret = bufferevent_socket_connect(bev, reinterpret_cast<sockaddr*>(addr), sizeof(*addr));
    if(ret < -1) {
        std::cerr << "connect other serve failed!" << std::endl;
    }
    bufferevent_setcb(bev, readCB, NULL, beventCB, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    return bev;
}

void WebServer::listenCB(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socklen, void* usr_data) {
    event_base* base = reinterpret_cast<event_base*>(usr_data);
    bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev) {
        std::cerr << "Create buffer event failed" << std::endl;
        event_base_loopbreak(base);
        exit(1);
    }
    bufferevent_setcb(bev, readCB, NULL, beventCB, base);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void WebServer::readCB(bufferevent* bev, void* ctx) {}

void WebServer::beventCB(bufferevent* bev, short what, void* ctx) {
    if(what & BEV_EVENT_EOF) {  //客户端关闭
        printf("connet closed\n");
        bufferevent_free(bev);
    } else if(what & BEV_EVENT_ERROR) {
        printf("closed error\n");
        bufferevent_free(bev);
    } else if(what & BEV_EVENT_CONNECTED) {  //连接成功
        printf("connect ok\n");
    }
}