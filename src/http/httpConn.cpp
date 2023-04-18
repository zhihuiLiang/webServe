#include "http/httpConn.h"

HttpConn::~HttpConn(){
    bufferevent_free(bev_);
}

void HttpConn::init(int fd, bufferevent* bev, sockaddr_in* addr) {
    fd_ = fd;
    addr_ = *addr;
    bev_ = bev;
}

void HttpConn::processReq(evbuffer* ev){
    size_t len = 0;
    if ((len = evbuffer_get_length(ev)) <= 0 )
        return;
    request_.init();
}

int HttpConn::getFd(){
    return fd_;
}

