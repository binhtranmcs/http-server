cmake_minimum_required(VERSION 3.24)
project(http_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(http_server main.cpp)

target_link_libraries(http_server PUBLIC http)

add_subdirectory(src/http)
