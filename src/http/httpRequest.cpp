#include "http/httpRequest.h"
#include "log/log.h"

#include <regex>

std::vector<std::string> HttpRequest::AVILIBLE_HTML = { "/index", "/register", "/login", "/welcome", "/video", "/picture" };

void HttpRequest::init() {
    method_ = path_ = version_ = "";

    header_.SetNull(); // 把根节点设为 null JSON 类型
    header_.GetAllocator().Clear(); // 清空 allocator 
    
    state_ = PARASE_STATE::REQUEST_LINE;
}

bool HttpRequest::parase(evbuffer* buf) {
    while(state_ != PARASE_STATE::FINISH) {
        size_t len = 0;
        char* line = evbuffer_readln(buf, &len, EVBUFFER_EOL_CRLF);
    
        int remain_size = 0;
        switch(state_) {
            case REQUEST_LINE:
                if(!paraseReqLine(line)) {
                    return false;
                }
                parasePath();
                break;
            case HEADER:
                paraseHeader(line);
                if((remain_size = evbuffer_get_length(buf)) < 2) {
                    state_ = FINISH;
                }
            case BODY:
                state_ = FINISH;
                break;
            default:
                break;
        }
    }
    return true;
}

bool HttpRequest::paraseReqLine(std::string line) {
    std::regex patten("^([^ ]) ([^ ]*) HTTP/([^ ])$");
    std::smatch sub_match;
    if(std::regex_match(line, sub_match, patten)) {
        method_ = sub_match[1];
        path_ = sub_match[2];
        version_ = sub_match[3];
        state_ = PARASE_STATE::HEADER;
        return true;
    } else {
        return false;
    }
}

void HttpRequest::parasePath() {
    if(path_ == "/") {
        path_ = "/index.html";
    } else {
        for(auto&& item : AVILIBLE_HTML) {
            if(item == path_) {
                path_ += ".html";
                break;
            }
        }
    }
}

void HttpRequest::paraseHeader(std::string line) {
    std::regex patten("^([^:]) ?(.*)$");
    std::smatch sub_match;
    if(std::regex_match(line, sub_match, patten)) {
        std::string key = sub_match[0], val = sub_match[1];
        modifyDoc(header_, key.c_str(), val.c_str());
    } else {
        state_ = BODY;
    }
}
