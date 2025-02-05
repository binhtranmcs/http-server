//
// Created by binhtt4 on 04/02/25.
//


#include "request_handler.h"

#include <cassert>
#include <iostream>

#include "epoll_handler.h"


namespace net {

void RequestHandler::Handle(uint32_t events) {
  if (events & EPOLLIN) {
    HandleRead();
  } else if (events & EPOLLOUT) {
    HandleWrite();
  } else {
    LOG_ERROR("events unrecognized " << events);
    assert(false);
  }
}


void RequestHandler::HandleRead() {
  while (true) {
    auto bytes_read = ::read(fd_, buffer_.data(), BUFFER_SIZE);
    if (bytes_read > 0) {
      request_buffer_.append(buffer_, 0, bytes_read);
      std::string buffer = protocol_handler_->ParseBuffer(request_buffer_);
      if (!buffer.empty()) {
        finished_ = protocol_handler_->Handle(buffer, response_buffer_);
        HandleWrite();
        if (finished_) {
          return;
        }
      }
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      epoll_event event{};
      event.events = EPOLLIN | EPOLLET;
      event.data.ptr = new EpollHandler(shared_from_this());
      epoll_->Add(fd_, &event);
      return;
    }
  }
}


void RequestHandler::HandleWrite() {
  uint32_t total_bytes_sent{0};
  while (total_bytes_sent < response_buffer_.size()) {
    auto bytes_sent = ::write(fd_, response_buffer_.c_str(), response_buffer_.size());
    if (bytes_sent > 0) {
      total_bytes_sent += bytes_sent;
    } else if (bytes_sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      epoll_event event{};
      event.events = EPOLLOUT | EPOLLET;
      event.data.ptr = new EpollHandler(shared_from_this());
      epoll_->Add(fd_, &event);
      return;
    } else {
      return;
    }
  }
}


} // namespace net
