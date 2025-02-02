
add_library(http STATIC HttpServer.cc
        epoll_server.cc
        epoll_server.h
        error_utils.h
        epoll_manager.cc
        epoll_manager.h
        utils.h)
