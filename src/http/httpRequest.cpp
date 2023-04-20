#include "http/httpRequest.h"
#include "sql/sqlconnpool.h"
#include "sql/sqlconnRAII.h"
#include "log/log.h"

#include <regex>

std::vector<std::string> HttpRequest::AVILIBLE_HTML = { "/index", "/register", "/login", "/welcome", "/video", "/picture" };
const std::unordered_map<std::string, int> HttpRequest::DEFAULT_HTML_TAG {
            {"/register.html", 0}, {"/login.html", 1},  };

void HttpRequest::init() {
    method_ = path_ = version_ = "";

    header_.SetNull(); // 把根节点设为 null JSON 类型
    header_.GetAllocator().Clear(); // 清空 allocator 
    
    state_ = PARASE_STATE::REQUEST_LINE;
}

bool HttpRequest::parase(evbuffer* buf) {
    while(state_ != PARASE_STATE::FINISH) {
        size_t len, remain_size = 0;;
        char *line, *body;
        if (state_ != BODY){
            len = 0;
            line = evbuffer_readln(buf, &len, EVBUFFER_EOL_CRLF);
        }else{
            len = evbuffer_copyout(buf, body, evbuffer_get_length(buf));
        }
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
                paraseBody(body, len);
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


void HttpRequest::paraseBody(const char* data, size_t len){
    body_ = std::string(data, len);
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded") {
        paraseFromUrlencoded();
        if(DEFAULT_HTML_TAG.count(path_)) {
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            LOG_DEBUG("Tag:%d", tag);
            if(tag == 0 || tag == 1) {
                bool isLogin = (tag == 1);
                if(userVerify(post_["username"], post_["password"], isLogin)) {
                    path_ = "/welcome.html";
                } 
                else {
                    path_ = "/error.html";
                }
            }
        }
    } 
    
    state_ = FINISH;
    LOG_DEBUG("Body Info: %s, len: %d", data, len);
}

int HttpRequest::converHex(char ch) {
    if(ch >= 'A' && ch <= 'F') return ch -'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch -'a' + 10;
    return ch;
}

void HttpRequest::paraseFromUrlencoded() {
    if(body_.size() == 0) { return; }

    std::string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;

    for(; i < n; i++) {
        char ch = body_[i];
        switch (ch) {
        case '=':
            key = body_.substr(j, i - j);
            j = i + 1;
            break;
        case '+':
            body_[i] = ' ';
            break;
        case '%':
            num = converHex(body_[i + 1]) * 16 + converHex(body_[i + 2]);
            body_[i + 2] = num % 10 + '0';
            body_[i + 1] = num / 10 + '0';
            i += 2;
            break;
        case '&':
            value = body_.substr(j, i - j);
            j = i + 1;
            post_[key] = value;
            LOG_DEBUG("%s = %s", key.c_str(), value.c_str());
            break;
        default:
            break;
        }
    }
    assert(j <= i);
    if(post_.count(key) == 0 && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

bool HttpRequest::userVerify(std::string name, std::string pwd, bool is_login) {
    if(name == "" || pwd == "") { return false; }
    LOG_INFO("Verify name:%s pwd:%s", name.c_str(), pwd.c_str());
    MYSQL* sql;
    SqlConnRAII(&sql,  SqlConnPool::Instance());
    assert(sql);
    
    bool flag = false;
    unsigned int j = 0;
    char order[256] = { 0 };
    MYSQL_FIELD *fields = nullptr;
    MYSQL_RES *res = nullptr;
    
    if(!isLogin) { flag = true; }
    /* 查询用户及密码 */
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());
    LOG_DEBUG("%s", order);

    if(mysql_query(sql, order)) { 
        mysql_free_result(res);
        return false; 
    }
    res = mysql_store_result(sql);
    j = mysql_num_fields(res);
    fields = mysql_fetch_fields(res);

    while(MYSQL_ROW row = mysql_fetch_row(res)) {
        LOG_DEBUG("MYSQL ROW: %s %s", row[0], row[1]);
        string password(row[1]);
        /* 注册行为 且 用户名未被使用*/
        if(isLogin) {
            if(pwd == password) { flag = true; }
            else {
                flag = false;
                LOG_DEBUG("pwd error!");
            }
        } 
        else { 
            flag = false; 
            LOG_DEBUG("user used!");
        }
    }
    mysql_free_result(res);

    /* 注册行为 且 用户名未被使用*/
    if(!isLogin && flag == true) {
        LOG_DEBUG("regirster!");
        bzero(order, 256);
        snprintf(order, 256,"INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());
        LOG_DEBUG( "%s", order);
        if(mysql_query(sql, order)) { 
            LOG_DEBUG( "Insert error!");
            flag = false; 
        }
        flag = true;
    }
    SqlConnPool::Instance()->FreeConn(sql);
    LOG_DEBUG( "UserVerify success!!");
    return flag;
}