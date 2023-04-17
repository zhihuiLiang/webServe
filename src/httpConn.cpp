#include "http/httpConn.h"

void httpConn::init(int fd, sockaddr_in* addr) {
    fd_ = fd;
    addr_ = *addr;
}