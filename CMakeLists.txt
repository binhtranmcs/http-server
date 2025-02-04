cmake_minimum_required(VERSION 3.19)
project(http_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(http_server main.cc)

target_link_libraries(http_server PUBLIC epoll_server)

add_subdirectory(src)
