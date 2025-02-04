

#include "src/epoll_server.h"


// echo -e binh | nc localhost 8080
int main() {
  net::EpollServer server(8080, 1);
  server.Start();
}

// runnable http server with benchmark
// learn io uring: unixism
// io uring vs linux socket?
// design linux socket impl
// impl using linux socket
// design io uring impl
// impl using io uring
// another variant with coroutines?
