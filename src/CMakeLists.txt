
find_package(Threads REQUIRED)

add_library(epoll_server STATIC
        epoll_manager.cc
        http_handler.cc
)

target_link_libraries(epoll_server PUBLIC Threads::Threads)

target_compile_options(epoll_server PRIVATE -Werror -Wall -Wextra)
