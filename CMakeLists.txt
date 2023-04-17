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
    src/httpConn.cpp
)

include_directories(
    INCLUDE_DIRS 
    include
)

add_executable(webServer ${SOURCE})
target_link_libraries(webServer event pthread)
