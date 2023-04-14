cmake_minimum_required(VERSION 3.1.2)
project(web_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE src/main.cpp src/web_server.cpp src/utility.cpp)

include_directories(
    INCLUDE_DIRS 
    include
)

add_executable(web_server ${SOURCE})
target_link_libraries(web_server event)
