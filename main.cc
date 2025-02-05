

#include "src/epoll_server.h"
#include "src/http_handler.h"
#include "src/protocol_handler.h"


// echo -e binhtran | nc localhost 8080
// curl http://localhost:8080/hello -d binhtranmcs
// curl http://localhost:8080/hello
int main() {
  auto handler = std::make_shared<net::HttpHandler>();
  handler->AddHandler("/hello", net::HttpMethod::GET,
      [](const net::HttpRequest& req, net::HttpResponse& res) -> bool {
    res.SetBody(req.HandlerKey() + " Hello World!\n");
    return true;
  });
  handler->AddHandler("/hello", net::HttpMethod::POST,
      [](const net::HttpRequest& req, net::HttpResponse& res) -> bool {
    res.SetBody(req.HandlerKey() + " Hello " + req.Body() + "!\n");
    return true;
  });

  auto server = net::EpollServer<net::HttpHandler>(8080, 1, handler);
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
