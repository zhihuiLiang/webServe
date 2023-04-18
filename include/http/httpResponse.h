#pragma once
#include <event2/buffer.h>
#include <event2/bufferevent.h>

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

    bool parase(evbuffer* buffer);

private:
    bool parseREQLINE(const char* line);
    bool parseHeader(const char* line);
    bool parseBody(const char* line);

};