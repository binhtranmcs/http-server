//
// Created by binhtt4 on 09/12/23.
//

#include "HttpServer.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <utility>

namespace network {

HttpServer::HttpServer(std::string address, int port, int num_threads)
    : address_(std::move(address))
    , port_(port)
    , num_threads_(num_threads)
    , events_(MAX_EPOLL_EVENTS) {
  if ((server_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
    throw std::runtime_error("create socket failed");
  }
  // TODO
  //  struct timeval timeout{};
  //  timeout.tv_sec = 10;
  //  timeout.tv_usec = 100000;
  //  if (setsockopt(server_fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
  //    throw std::runtime_error("setsockopt failed");
  //  }
  //  if (setsockopt(server_fd_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
  //    throw std::runtime_error("setsockopt failed");
  //  }
}

HttpServer::~HttpServer() {
  event_thread_.join();
}

void HttpServer::StartServer() {
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(port_);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd_, (sockaddr *)&address, sizeof(address)) < 0) {
    throw std::runtime_error("bind failed");
  }
  if (listen(server_fd_, 0) < 0) {
    throw std::runtime_error("listen failed");
  }

  //  worker_threads_.reserve(num_threads_);
  //  for (int id = 0; id < num_threads_; ++id) {
  //    if ((worker_epoll_fd_[id] = epoll_create1(0)) < 0) {
  //      throw std::runtime_error("create worker fd failed");
  //    }
  //    worker_threads_.emplace_back(&HttpServer::WorkerLoop, this, id);
  //  }

  event_thread_ = std::thread(&HttpServer::EventLoop, this);
}

void HttpServer::WorkerLoop(int id) {
  //  int worker_fd = worker_epoll_fd_[id];
  //  auto &worker_events = worker_events_[id];
  //  while (true) {
  //    int nfds = epoll_wait(worker_fd, worker_events.data(), MAX_EPOLL_EVENTS, 0);
  //    if (nfds < 0) {
  //      throw std::runtime_error("epoll_wait failed");
  //    }
  //
  //    for (int i = 0; i < nfds; ++i) {
  //      epoll_event &event = worker_events[i];
  //      if (event.events == EPOLLIN) {
  //        HandleEvent(worker_fd, event);
  //      } else {
  //        close(event.data.fd);
  //      }
  //    }
  //  }
}

void HttpServer::EventLoop() {
  sockaddr_in client_address{};
  socklen_t client_len = sizeof(client_address);

  int worker_id{};
  int client_fd{};

  while (true) {
    int nfds = epoll_wait(epoll_fd_, events_.data(), MAX_EPOLL_EVENTS, 0);
    if (nfds < 0) {
      throw std::runtime_error("epoll_wait failed");
    }

    for (int i = 0; i < nfds; ++i) {
      HandleEvent(events_[i]);
    }

    client_fd = accept4(server_fd_, (sockaddr *)&client_address, &client_len, 0);
    if (client_fd < 0) {
      //      std::cout << "error " << client_fd << ' ' << errno << std::endl;
      continue;
    }
    std::cout << "??????????" << std::endl;

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = client_fd;

    // if (epoll_ctl(worker_epoll_fd_[worker_id], EPOLL_CTL_ADD, client_fd, &event) < 0) {
    //   std::cout << "add client failed" << std::endl;
    // }

    worker_id = (worker_id == num_threads_ - 1) ? 0 : worker_id + 1;
  }
}

void HttpServer::HandleEvent(epoll_event& event) {
  if (event.data.fd == server_fd_) {
    // this is a new connection
    HandleAcceptEvent(event);
  }
  if (event.events == EPOLLIN) {
    HandleReadEvent(event);
  } else {
    HandleWriteEvent(event);
  }
}

void HttpServer::HandleAcceptEvent(epoll_event& /*event*/) const {
  sockaddr_in client_address{};
  socklen_t client_len = sizeof(client_address);

  int client_fd = accept4(server_fd_, (sockaddr *)&client_address, &client_len, 0);
  if (client_fd < 0) {
    std::cout << "accept error " << client_fd << ' ' << errno << std::endl;
    return;
  }
  // btmcs TODO set nonblocking?

  epoll_event conn_event{};
  conn_event.events = EPOLLIN;
  conn_event.data.fd = client_fd;

  if (epoll_ctl(server_fd_, EPOLL_CTL_ADD, client_fd, &conn_event) < 0) {
    std::cout << "add client failed" << std::endl;
  }
}

void HttpServer::HandleReadEvent(epoll_event& event) {
}

void HttpServer::HandleWriteEvent(epoll_event& event) {
}

} // namespace network
