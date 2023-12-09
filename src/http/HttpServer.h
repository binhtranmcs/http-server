//
// Created by binhtt4 on 09/12/23.
//

#ifndef HTTP_SERVER_HTTPSERVER_H
#define HTTP_SERVER_HTTPSERVER_H

#include <string>
#include <functional>
#include <thread>
#include "sys/epoll.h"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpMethod.h"

namespace network {

const int MAX_EPOLL_EVENTS = 10000;

using HttpHandler = std::function<HttpResponse>(const HttpRequest &);

class HttpServer {
public:
  HttpServer(std::string address, int port, int num_threads);

  ~HttpServer();

  void StartServer();

  void ListenerLoop();

  void WorkerLoop(int id);

  void AddHandler(const std::string &path, HttpMethod method, HttpHandler callback);

private:
  void HandleEvent(int worker_fd, epoll_event event);

  std::string address_;
  int port_;

  int server_fd_;

  int num_threads_;
  std::vector<int> worker_epoll_fd_;
  std::vector<std::vector<epoll_event>> worker_events_;
  std::vector<std::thread> worker_threads_;
  std::thread listener_thread_;
};

} // namespace http

#endif // HTTP_SERVER_HTTPSERVER_H
