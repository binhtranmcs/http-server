
add_library(epoll_server STATIC
        epoll_server.cc
        epoll_manager.cc
        request_handler.cc
        protocol_handler.cc
)
