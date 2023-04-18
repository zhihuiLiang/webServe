#include "http/httpRequest.h"
#include "log/log.h"

#include <regex>


std::vector<std::string> HttpRequest::AVILIBLE_HTML = {
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture"};

void HttpRequest::init(){
    method_ = path_ = version_ = "";
    head_.Clear();
    state_ = PARASE_STATE::REQUEST_LINE;
}


bool HttpRequest::parase(evbuffer* bfer){
    while(state_ != PARASE_STATE::FINISH){  
        size_t len = 0;
        char* line = evbuffer_readln(bfer, &len, EVBUFFER_EOL_CRLF);
    
        switch (state_){
        case REQUEST_LINE:
            if (!paraseReqLine(line)){
                return false;
            }
            parasePath();
            break;
        case HEADER:
            if (paraseHeader(line))
        default:
            break;
        }
    }
}

bool HttpRequest::paraseReqLine(std::string line){
    std::regex patten("^([^ ]) ([^ ]*) HTTP/([^ ])$");
    std::smatch sub_match;
    if (std::regex_match(line, sub_match, patten)){
        method_ = sub_match[1];
        path_ = sub_match[2];
        version_ = sub_match[3];
        state_ = PARASE_STATE::HEADER;
        return true;
    }
    LOG_ERROR("Parase Request line failed!");
    return false;
}

void HttpRequest::parasePath(){
    if (path_ == "/"){
        path_ = "/index.html";
    }else{
        for (auto&& item : AVILIBLE_HTML){
            if (item == path_){
                path_ += ".html";
                break;
            }
        }
    }
}

bool HttpRequest::paraseHeader(std::string line){
    std::regex patten("^([^:]) ?(.*)$");
    std::smatch sub_match;
    if (std::regex_match(line, sub_match, patten)){
        return modifyDoc(sub_match[0], sub_match[1]);
    }
    return false;
}
