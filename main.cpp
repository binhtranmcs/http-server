#include <iostream>
#include "src/http/HttpServer.h"

int main() {
  network::HttpServer server("0.0.0.0", 8080, 4);
  server.StartServer();
}

// vscode setup for c++
// learn io uring: unixism
// io uring vs linux socket?
// design linux socket impl
// impl using linux socket
// design io uring impl
// impl using io uring
// another variant with coroutines?
