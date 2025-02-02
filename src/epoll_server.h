//
// Created by binhtranmcs on 01/02/2025.
//

#ifndef HTTP_SERVER_SRC_EPOLL_SERVER_H_
#define HTTP_SERVER_SRC_EPOLL_SERVER_H_


#include <thread>
#include <vector>

#include "epoll_manager.h"


namespace tcp {


class EpollServer {
public:
  explicit EpollServer(int port, int num_worker_threads = 4);


  [[noreturn]] [[noreturn]] void AcceptLoop();

  void WorkerLoop(int worker_id);

private:
  int server_fd_;
  int port_;

  std::jthread accept_thread_;
  EpollManager server_epoll_;

  int num_worker_threads_;
  std::vector<std::jthread> workers_;
  std::vector<EpollManager> worker_epolls_;
};


} // namespace tcp


#endif // HTTP_SERVER_SRC_EPOLL_SERVER_H_
