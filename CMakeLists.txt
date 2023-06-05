cmake_minimum_required(VERSION 3.1.2)
project(web_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

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
    src/timeWheel.cpp
)

set(MYSQL_LIB mysqlclient mysqld mysqlservices)

include_directories(
    INCLUDE_DIRS 
    include
)

add_executable(webServer ${SOURCE})
target_include_directories(webServer PRIVATE  $ENV{MY_SQL}/include)
target_link_libraries(webServer event pthread ${MYSQL_LIB})
