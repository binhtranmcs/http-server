#include <iostream>
#include "src/http/HttpServer.h"

int main() {
  network::HttpServer server("0.0.0.0", 8080, 4);
  server.StartServer();
}
