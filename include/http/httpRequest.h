#pragma once
#include "utility.h"

#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <string>
#include <vector>
#include <unordered_map>

class HttpRequest{
public:
    enum PARASE_STATE{
        REQUEST_LINE,
        HEADER,
        BODY,
        FINISH
    };

    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    void init();
    bool parase(evbuffer* buffer);

private:
    bool paraseReqLine(std::string line);
    bool paraseHeader(std::string line);
    bool paraseBody(std::string line);

    void parasePath();

    PARASE_STATE state_;

    std::string method_, path_, version_;
    rapidjson::Document head_;

    static std::vector<std::string> AVILIBLE_HTML;
};