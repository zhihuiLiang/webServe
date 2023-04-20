#pragma once
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <string>
#include <unordered_map>

class HttpResponse{
public:
    HttpResponse();
    void init(std::string srcDir, std::string path, bool isKeepAlive, int code);
    void makeReSponse(evbuffer* buf);

    static std::unordered_map<int, std::string> CODE_STATUS;
    static std::unordered_map<int, std::string> CODE_PATH;
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;

private:
    std::string path_;
    std::string src_dir_;

    bool is_keep_alive_;
    int code_;

    struct stat file_stat_;

    std::string getFileType();
    int bufWrite(evbuffer* buff, std::string data);

    void errorHtml();
    void errorContent(evbuffer* buf, std::string message);
    void addStateLine(evbuffer* buf);
    void addHeader(evbuffer* buf);
    void addContent(evbuffer* buf);
};