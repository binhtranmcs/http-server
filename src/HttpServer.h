//
// Created by binhtt4 on 09/12/23.
//

#ifndef HTTP_SERVER_HTTPSERVER_H
#define HTTP_SERVER_HTTPSERVER_H

#include <functional>
#include <string>
#include <thread>

#include "HttpMethod.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "sys/epoll.h"

namespace network {

const int MAX_EPOLL_EVENTS = 10000;

using HttpHandler = std::function<HttpResponse>(const HttpRequest&);

class HttpServer {
public:
  HttpServer(std::string address, int port, int num_threads);

  ~HttpServer();

  void StartServer();

  void InitIOThreadPool();

  void EventLoop();

  void WorkerLoop(int id);

  void AddHandler(const std::string& path, HttpMethod method, HttpHandler callback);

private:
  void HandleEvent(epoll_event& event);

  void HandleAcceptEvent(epoll_event& event) const;

  void HandleReadEvent(epoll_event& event);

  void HandleWriteEvent(epoll_event& event);

  std::string address_;
  int port_;

  int server_fd_;
  int epoll_fd_;

  int num_threads_;
  std::vector<epoll_event> events_;
  std::thread event_thread_;
};

} // namespace network

#endif // HTTP_SERVER_HTTPSERVER_H
