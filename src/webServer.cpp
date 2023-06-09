#include "webServer/webServer.h"
#include "log/log.h"

#include <future>
#include <chrono>
#include <thread>

using namespace std::literals;

void beventCB(bufferevent* bev, short what, void* ctx) {
    HttpConn* client = reinterpret_cast<HttpConn*>(ctx);
    char ip[16] = {0};

    if(what & BEV_EVENT_EOF) {  // 客户端关闭
        LOG_INFO("IP: %s, connet closed", client->getIP());
        bufferevent_free(bev);
    } else if(what & BEV_EVENT_ERROR) {
        LOG_INFO("IP: %s, connet closed error", client->getIP());
        bufferevent_free(bev);
    } else if(what & BEV_EVENT_CONNECTED) {  // 连接成功
        printf("connect ok\n");
    }
}

void readCB(bufferevent* bev, void* ctx) {
    auto client = reinterpret_cast<HttpConn*>(ctx);
    client->processReq();
}

void conSrvCB(bufferevent* bev, void* ctx){
    auto client = reinterpret_cast<HttpConn*>(ctx);
    size_t len = 0;
    evbuffer* buf = bufferevent_get_input(bev);
    char *line = evbuffer_readln(buf, &len, EVBUFFER_EOL_CRLF);
    while(len > 0){
        std::string recv_line(line);
        LOG_INFO("Receive IP: %s response: %s", client->getIP(), line);
        line = evbuffer_readln(buf, &len, EVBUFFER_EOL_CRLF);
    }
}


void listenCB(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socklen, void* usr_data) {
    WebServer* srv = reinterpret_cast<WebServer*>(usr_data);
    ++HttpConn::user_cnt_;

    bufferevent* bev = bufferevent_socket_new(srv->base_, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev) {
        LOG_ERROR("Listener create buffer event failed!");
        event_base_loopbreak(srv->base_);
        exit(1);
    }
    char ip[16] = { 0 };
    inet_ntop(AF_INET, reinterpret_cast<void*>(sa), ip, sizeof(sockaddr));
    srv->users_[fd].init(fd, bev, ip);
    LOG_INFO("IP: %s, Client[%d] log try to connect.", ip, fd);

    bufferevent_setcb(bev, readCB, NULL, beventCB, &(srv->users_[fd]));
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

WebServer::WebServer(int port) : base_(nullptr) {
    src_dir_ = getcwd(nullptr, 256);
    assert(src_dir_);
    strncat(src_dir_, "/resource", 16);
    HttpConn::src_dir_ = src_dir_;

    Log::Instance()->init(1, "./log", ".log", 0);

    base_ = event_base_new();

    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = INADDR_ANY;

    listener_ = evconnlistener_new_bind(base_, listenCB, this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                        reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_));
    if(!listener_) {
        LOG_ERROR("Create lisener failed:%s", strerror(errno));
        exit(1);
    }

    auto fut = std::async(&WebServer::addTask, this);
    event_base_dispatch(base_);
}

int WebServer::connectSrv(std::string host, int port) {
    sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), (&srv_addr.sin_addr));
    bufferevent* bev = bufferevent_socket_new(base_, -1, BEV_OPT_CLOSE_ON_FREE);
    int ret = bufferevent_socket_connect(bev, reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr));
    if(ret < -1) {
        LOG_ERROR("connect other server failed!");
    }
    int fd = bufferevent_getfd(bev);
    if (fd < 0){
        LOG_ERROR("Get Connected fd failed!");
    }
    users_[fd].init(fd, bev, host.c_str());

    bufferevent_setcb(bev, conSrvCB, NULL, beventCB, &users_[fd]);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    return fd;
}

void WebServer::addTask(){
    AutoReserve task;  //ip 8.129.5.136
    while(1){
        auto target = task.getNextTime();
        std::this_thread::sleep_until(target); 

        const char* ip = "8.129.5.136";
        auto fd = connectSrv(ip, 80);

        auto [path, header, contents] = task.reserve(target);

        int cnt = 2;
        while(cnt--){
            for (const auto& content : contents){
                users_[fd].sendReq("POST", path, header, content);
            }
            std::this_thread::sleep_for(1s);
        }
    }

}

WebServer::~WebServer() {
    evconnlistener_free(listener_);
    event_base_free(base_);
}
