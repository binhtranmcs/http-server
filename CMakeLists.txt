cmake_minimum_required(VERSION 3.19)
project(http_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(http_server main.cpp)

target_link_libraries(http_server PUBLIC http)

add_subdirectory(src)
