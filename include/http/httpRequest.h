#pragma once
#include "utility.h"

#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <string>
#include <unordered_map>
#include <vector>

class HttpRequest {
public:
    enum PARASE_STATE { REQUEST_LINE, HEADER, BODY, FINISH };

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
    void paraseFromUrlencoded();
    int converHex(char ch);

    bool userVerify(std::string name, std::string pwd, bool is_login);
    std::string makeRequest(std::string method, std::string path, const std::string& header, const std::string& body);

private:
    bool paraseReqLine(std::string line);
    void paraseHeader(std::string line);
    void paraseBody(const char* data, size_t len);

    void parasePath();

    PARASE_STATE state_;

    std::unordered_map<std::string, std::string> post_;
    std::string method_, path_, version_, body_;
    rapidjson::Document header_;

    static std::vector<std::string> AVILIBLE_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;

    friend class HttpConn;
};