//
// Created by binhtranmcs on 01/02/2025.
//

#include "epoll_server.h"

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <cassert>
#include <memory>
#include <thread>

#include "epoll_handler.h"
#include "error_utils.h"
#include "request_handler.h"
#include "utils.h"


namespace net {


// TODO: template the protocol handler
EpollServer::EpollServer(int port, int num_worker_threads)
    : port_(port)
    , num_worker_threads_(num_worker_threads)
    , worker_epolls_(num_worker_threads) {
  // create a tcp socket
  THROW_IF_ERROR_LT_0(server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0), "::socket failed");
  int opt = 1;
  THROW_IF_ERROR_LT_0(::setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)),
      "::setsockopt failed");

  // create server ipv4 address
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_);

  // bind the file descriptor to the created address
  THROW_IF_ERROR_LT_0(
      ::bind(server_fd_, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)),
      "::bind failed");

  // start listening on the server fd
  THROW_IF_ERROR_LT_0(::listen(server_fd_, SOMAXCONN), "::listen failed");

  // create worker threads to monitor
  for (int id = 0; id < num_worker_threads_; id++) {
    worker_epolls_[id] = std::make_shared<EpollManager>();
    workers_.emplace_back(&EpollServer::WorkerLoop, this, id);
  }

  // add server fd to interest list
  epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = server_fd_;
  server_epoll_.Add(server_fd_, &event);
}


[[noreturn]] void EpollServer::AcceptLoop() {
  int worker_id = 0;
  auto& events = server_epoll_.Events();
  while (true) {
    int num_fds = server_epoll_.Wait();
    if (num_fds == 0) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    std::cout << __func__ << std::endl;
    for (int i = 0; i < num_fds; i++) {
      auto& event = events[i];
      assert(event.data.fd == server_fd_);
      // get client fd to read from or write to
      int client_fd = ::accept(server_fd_, nullptr, nullptr);
      if (client_fd == -1) {
        std::cerr << "error\n";
        // TODO: is this correct?
        continue;
      }
      utils::SetNonBlocking(client_fd);
      auto worker_epoll = worker_epolls_[worker_id];
      // create new request context to manage client_fd
      auto epoll_handler =
          new EpollHandler(std::make_shared<RequestHandler>(client_fd, worker_epoll));
      epoll_event client_event{};
      client_event.events = EPOLLIN | EPOLLET;
      client_event.data.ptr = epoll_handler;

      // TODO: if this Add() failed, should I ignore or do something else?
      worker_epoll->Add(client_fd, &client_event);
      worker_id = (++worker_id == num_worker_threads_) ? 0 : worker_id;
    }
  }
}


void EpollServer::WorkerLoop(int worker_id) {
  auto worker_epoll = worker_epolls_[worker_id];
  auto& events = worker_epoll->Events();
  while (true) {
    int num_fds = worker_epoll->Wait();
    if (num_fds == 0) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    std::cout << __func__ << std::endl;
    for (int i = 0; i < num_fds; i++) {
      auto event = events[i];
      auto *epoll_handler = static_cast<EpollHandler *>(event.data.ptr);
      epoll_handler->Proceed(event.events);
      delete epoll_handler;
    }
  }
}


} // namespace net
