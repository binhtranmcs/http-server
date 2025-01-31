//
// Created by binhtranmcs on 01/02/2025.
//

#include "epoll_server.h"

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <thread>

#include "error_utils.h"


namespace tcp {

EpollServer::EpollServer(int port, int num_worker_threads)
    : port_(port)
    , num_worker_threads_(num_worker_threads) {
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
  for (int i = 0; i < num_worker_threads_; ++i) {
    workers_.emplace_back(&EpollServer::WorkerLoop, this);
  }
}


void EpollServer::WorkerLoop() {
  while (true) {}
}


void EpollServer::AcceptLoop() {
  int worker_id = 0;
  auto& events = server_epoll_.Events();
  while (true) {
    int num_fds = server_epoll_.Wait();
    for (int i = 0; i < num_fds; i++) {
      auto& event = events[i];
      auto *request_ctx = static_cast<RequestContext *>(event.data.ptr);
      if (request_ctx->fd_ == server_fd_) {
        int client_fd = ::accept(server_fd_, nullptr, nullptr);
        if (client_fd == -1) {
          continue;
        }

        worker_epolls_[worker_id].Add(client_fd);
        worker_id = (++worker_id == num_worker_threads_) ? 0 : worker_id;
      }
    }
  }
}


} // namespace tcp