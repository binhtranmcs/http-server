//
// Created by binhtt4 on 04/02/25.
//

#ifndef HTTP_SERVER_SRC_REQUEST_HANDLER_H
#define HTTP_SERVER_SRC_REQUEST_HANDLER_H


#include <unistd.h>

#include <algorithm>
#include <memory>

#include "epoll_manager.h"
#include "epoll_server.h"
#include "protocol_handler.h"


namespace net {


struct EpollHandler;


template <IProtocolHandler THandler>
class RequestHandler : public std::enable_shared_from_this<RequestHandler<THandler>> {
public:
  explicit RequestHandler(int fd, std::shared_ptr<EpollManager> epoll)
      : fd_(fd)
      , epoll_(std::move(epoll)) {
    buffer_.resize(BUFFER_SIZE);
  }

  ~RequestHandler() {
    ::close(fd_);
  }

  void Handle(uint32_t events);

private:
  void HandleRead();

  void HandleWrite();

  std::shared_ptr<RequestHandler<THandler>> SharedFromThis() {
    return this->shared_from_this();
  }

  static constexpr int BUFFER_SIZE = 4096;

  int fd_;
  std::string request_buffer_;
  std::string buffer_;

  std::string response_buffer_;
  bool finished_{false};

  std::shared_ptr<EpollManager> epoll_;
  std::shared_ptr<THandler> protocol_handler_;
};


template <IProtocolHandler THandler>
void RequestHandler<THandler>::Handle(uint32_t events) {
  if (events & EPOLLIN) {
    HandleRead();
  } else if (events & EPOLLOUT) {
    HandleWrite();
  } else {
    LOG_ERROR("events unrecognized " << events);
    assert(false);
  }
}


template <IProtocolHandler THandler>
void RequestHandler<THandler>::HandleRead() {
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
      event.data.ptr = new EpollHandler(SharedFromThis());
      epoll_->Add(fd_, &event);
      return;
    }
  }
}


template <IProtocolHandler THandler>
void RequestHandler<THandler>::HandleWrite() {
  uint32_t total_bytes_sent{0};
  while (total_bytes_sent < response_buffer_.size()) {
    auto bytes_sent = ::write(fd_, response_buffer_.c_str(), response_buffer_.size());
    if (bytes_sent > 0) {
      total_bytes_sent += bytes_sent;
    } else if (bytes_sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      epoll_event event{};
      event.events = EPOLLOUT | EPOLLET;
      event.data.ptr = new EpollHandler(SharedFromThis());
      epoll_->Add(fd_, &event);
      return;
    } else {
      return;
    }
  }
}


} // namespace net


#endif // HTTP_SERVER_SRC_REQUEST_HANDLER_H
