cmake_minimum_required(VERSION 3.1.2)
project(web_server)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    INCLUDE_DIRS 
    include
)

add_executable(web_server src/main.cpp)
target_link_libraries(web_server event)