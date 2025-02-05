cmake_minimum_required(VERSION 3.19)
project(http_server)

set(CMAKE_CXX_STANDARD 23)

# set log level
set(CMAKE_LOG_LEVEL "INFO" CACHE STRING "Set log level (DEBUG, INFO, WARN)")
if (CMAKE_LOG_LEVEL STREQUAL "DEBUG")
    add_definitions("-DLOG_LEVEL_DEBUG")
elseif (CMAKE_LOG_LEVEL STREQUAL "INFO")
    add_definitions("-DLOG_LEVEL_INFO")
elseif (CMAKE_LOG_LEVEL STREQUAL "WARN")
    add_definitions("-DLOG_LEVEL_WARN")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(http_server main.cc)

target_link_libraries(http_server PUBLIC epoll_server)

target_compile_options(http_server PRIVATE -Werror -Wall -Wextra)

add_subdirectory(src)
