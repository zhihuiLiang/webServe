#include "log/log.h"
#include "web_server/web_server.h"

#include <functional>

void listenCB(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socklen, void* usr_data) {
    WebServe* srv = reinterpret_cast<WebServer*>(usr_data);
    ++WebServer::http_cnt_;


    bufferevent* bev = bufferevent_socket_new(srv->base_, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev) {
        LOG_ERROR("Listener create buffer event failed!");
        event_base_loopbreak(base);
        exit(1);
    }
    bufferevent_setcb(bev, readCB, NULL, beventCB, base);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

WebServer::WebServer(int port) : base_(nullptr) {
    using std::placeholders;

    event_base* base;
    base_ = event_base_new();

    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = INADDR_ANY;
    
    evconnlistener_cb callback;
    listener_ = evconnlistener_new_bind(base_, , (void*)base_, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                    reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_));
    if(!listener_) {
        LOG_ERROR("Create lisener failed");
        exit(1);
    }

    event_base_dispatch(base);
}

void WebServer::addClient(){
    
}


bufferevent* WebServer::connectSrv(std::string host, int port) {
    sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), (&srv_addr.sin_addr));
    bufferevent* bev = bufferevent_socket_new(base_, -1, BEV_OPT_CLOSE_ON_FREE);
    int ret = bufferevent_socket_connect(bev, reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr));
    if(ret < -1) {
        LOG_ERROR("connect other serve failed!");
    }
    bufferevent_setcb(bev, readCB, NULL, beventCB, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    return bev;
}

void WebServer::sendHttpReq(bufferevent* bev, std::string method, std::string path, std::string header, std::string content) {
    std::stringstream stream;
    stream << method << " " << path << " HTTP/1.1\r\n";
    stream << "Content-Length: " << content.size() << "\r\n";
    stream << header << "\r\n";
    stream << content.c_str();
    bufferevent_write(bev, stream.str().c_str(), stream.str().size());
}

void WebServer::listenCB(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socklen, void* usr_data) {
    event_base* base = reinterpret_cast<event_base*>(usr_data);

    bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev) {
        LOG_ERROR("Listener create buffer event failed!");
        event_base_loopbreak(base);
        exit(1);
    }
    bufferevent_setcb(bev, readCB, NULL, beventCB, base);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void WebServer::readCB(bufferevent* bev, void* ctx) {
    char bufline[256];
    int ret = 0;
    while((ret = bufferevent_read(bev, bufline, sizeof(bufline))) > 0){
        write(STDOUT_FILENO,bufline,ret);
    }
}

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

WebServer::~WebServer(){
    evconnlistener_free(listener_);
    event_base_free(base_);
}