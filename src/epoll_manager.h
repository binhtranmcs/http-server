//
// Created by binhtranmcs on 01/02/2025.
//

#ifndef HTTP_SERVER_SRC_EPOLL_MANAGER_H_
#define HTTP_SERVER_SRC_EPOLL_MANAGER_H_


#include <sys/epoll.h>
#include <sys/socket.h>

#include <vector>


namespace net {


class EpollManager {
public:
  EpollManager();

  int Wait();

  void Add(int fd, epoll_event *event) const;

  void Del(int fd) const;

  void Mod(int fd, epoll_event *event) const;

  [[nodiscard]] const std::vector<epoll_event>& Events() const {
    return events_;
  }

private:
  inline static const int MAX_EVENTS = SOMAXCONN;

  int epoll_fd_;
  std::vector<epoll_event> events_;
};


} // namespace net


#endif // HTTP_SERVER_SRC_EPOLL_MANAGER_H_
