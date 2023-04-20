#include "http/httpConn.h"
#include "log/log.h"

const char* HttpConn::src_dir_;
int HttpConn::user_cnt_ = 0;

void HttpConn::init(int fd, bufferevent* bev, char* ip) {
    fd_ = fd;
    bev_ = bev;
    std::copy(ip, ip + 16, ip_);
}

void HttpConn::processReq(){
    size_t len = 0;
    evbuffer* input_buf = bufferevent_get_input(bev_);
    if ((len = evbuffer_get_length(input_buf)) <= 0 )
        return;
    request_.init();
    if (request_.parase(input_buf)){
        LOG_INFO("%s request %s", ip_, request_.path_.c_str());
        response_.init(src_dir_, request_.path_, true, 200); //TODO keep_alive
    }else{
        response_.init(src_dir_, request_.path_, false, 400);
    }
    
    evbuffer* output_buf = bufferevent_get_output(bev_);
    response_.makeReSponse(output_buf);
    if (evbuffer_write(output_buf, fd_) > 0){
        LOG_INFO("Write :%d bytes to %s", ip_);
    }
}

int HttpConn::getFd(){
    return fd_;
}

char* HttpConn::getIP(){
    return ip_;
}

