
find_package(Threads REQUIRED)

add_library(epoll_server STATIC
        epoll_manager.cc
        protocol_handler.cc
)

target_link_libraries(epoll_server PUBLIC Threads::Threads)
