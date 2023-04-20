cmake_minimum_required(VERSION 3.1.2)
project(web_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE 
    src/main.cpp 
    src/webServer.cpp 
    src/utility.cpp 
    src/log.cpp
    src/buffer.cpp
    src/http/httpConn.cpp
    src/http/httpRequest.cpp
    src/http/httpResponse.cpp
    src/sqlconnpool.cpp
)

set(MYSQL_DIR $ENV{MYSQL_PATH})
message("mysql path:" ${MYSQL_DIR})

include_directories(
    INCLUDE_DIRS 
    ${MYSQP_DIR}/include
    include
)

add_executable(webServer ${SOURCE})
target_link_libraries(webServer event pthread)
