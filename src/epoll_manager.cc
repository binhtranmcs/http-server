//
// Created by binhtranmcs on 01/02/2025.
//

#include "epoll_manager.h"

#include "error_utils.h"


namespace net {


EpollManager::EpollManager() {
  THROW_IF_ERROR_LT_0(epoll_fd_ = ::epoll_create1(0), "::epoll_create1 failed");
  events_.resize(MAX_EVENTS);
}


void EpollManager::Add(int fd, epoll_event *event) const {
  THROW_IF_ERROR_LT_0(::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, event), "::epoll_ctl add failed");
}


void EpollManager::Del(int fd) const {
  THROW_IF_ERROR_LT_0(::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr), "::epoll_dtl del failed");
}


void EpollManager::Mod(int fd, epoll_event *event) const {
  THROW_IF_ERROR_LT_0(::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, event), "::epoll_dtl del failed");
}


int EpollManager::Wait() {
  int num_fds = ::epoll_wait(epoll_fd_, events_.data(), MAX_EVENTS, 0);
  THROW_IF_ERROR_LT_0(num_fds, "::epoll_wait failed");
  return num_fds;
}


} // namespace net
