#include "http/httpResponse.h"
#include "log/log.h"

#include <assert.h>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap

const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

std::unordered_map<int, std::string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};



HttpResponse::HttpResponse() : code_(-1), path_(""), src_dir_(""), is_keep_alive_(false){
}

void HttpResponse::init(std::string src_dir, std::string path, bool is_keep_alive, int code){
    assert(src_dir != "");
    assert(src_dir != "");
    code_ = code;
    is_keep_alive_ = is_keep_alive;
    path_ = path;
    src_dir_ = src_dir;
}

void HttpResponse::makeReSponse(evbuffer* buf){
    if (stat((src_dir_ + path_).c_str(), &file_stat_) < 0 || S_ISDIR(file_stat_.st_mode)){
        code_ = 404;
    }else if (!(file_stat_.st_mode & S_IROTH)){
        code_ = 403;
    }else if (code_ == -1){
        code_ = 200;
    }
    errorHtml();
    addStateLine(buf);
    addHeader(buf);
    addContent(buf);
}

void HttpResponse::errorHtml() {
    if(CODE_PATH.count(code_) == 1) {
        path_ = CODE_PATH.find(code_)->second;
        stat((src_dir_ + path_).data(), &file_stat_);
    }
}

void HttpResponse::addStateLine(evbuffer* buff) {
    std::string status;
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    }
    else {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
  
    if (bufWrite(buff, "HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n") < 0){
        LOG_ERROR("Write state line to buff failed!");
    }
}

void HttpResponse::addHeader(evbuffer* buf) {
    bufWrite(buf, "Connection: ");
    if(is_keep_alive_) {
        bufWrite(buf, "keep-alive\r\n");
        bufWrite(buf, "keep-alive: max=6, timeout=120\r\n");
    } else{
        bufWrite(buf, "close\r\n");
    }
    bufWrite(buf, "Content-type: " + getFileType() + "\r\n");
}

void HttpResponse::errorContent(evbuffer* buf, std::string message) 
{
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        status = "Bad Request";
    }
    body += std::to_string(code_) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";

    bufWrite(buf, "Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    bufWrite(buf, body);
}

void HttpResponse::addContent(evbuffer* buf) {
    int src_fd = open((src_dir_ + path_).data(), O_RDONLY);
    if(src_fd < 0) { 
        errorContent(buf, "File NotFound!");
        return; 
    }
    off_t size = lseek(src_fd, 0, SEEK_END);
    LOG_DEBUG("file path : %s, size : %d", (src_dir_ + path_).data(), size);
    evbuffer_add_file(buf, src_fd, 0, size);

    close(src_fd);
    bufWrite(buf, "Content-length: " + std::to_string(file_stat_.st_size) + "\r\n\r\n");
}

std::string HttpResponse::getFileType(){
    std::string::size_type idx = path_.find_last_of('.');
    if(idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = path_.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

int HttpResponse::bufWrite(evbuffer* buff, std::string data){
    return evbuffer_add(buff, data.c_str(), data.size());
}
