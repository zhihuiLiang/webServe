#include "http/httpConn.h"
#include "log/log.h"

const char* HttpConn::src_dir_;
int HttpConn::user_cnt_ = 0;

void HttpConn::init(int fd, bufferevent* bev, const char* ip) {
    fd_ = fd;
    bev_ = bev;
    std::copy(ip, ip + 16, ip_);

    task_finish = false;
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
    size_t size = evbuffer_get_length(output_buf);
    if (bufferevent_flush(bev_, EV_WRITE, BEV_FINISHED) >= 0){
        LOG_INFO("Write :%d bytes to %s", size, ip_);
    }else{
        LOG_INFO("Write wrong ocurred to %s: ", ip_, strerror(errno));
    }
}

void HttpConn::sendReq(std::string method, std::string path, const std::string& header, const std::string& body){
    std::string req = request_.makeRequest(method, path, header, body);
    if (bufferevent_write(bev_, req.c_str(), req.size()) >= 0){
        LOG_INFO("Send :%d bytes to %s", req.size(), ip_);
    }else{
        LOG_INFO("Write wrong ocurred to %s: %s", ip_, strerror(errno));
    }
}

int HttpConn::getFd(){
    return fd_;
}

char* HttpConn::getIP(){
    return ip_;
}

