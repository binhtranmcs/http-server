//
// Created by binhtt4 on 04/02/25.
//

#ifndef HTTP_SERVER_SRC_REQUEST_HANDLER_H
#define HTTP_SERVER_SRC_REQUEST_HANDLER_H


#include <algorithm>
#include <memory>

#include "epoll_manager.h"
#include "epoll_server.h"
#include "protocol_handler.h"


namespace net {


// TODO: use type erasure pattern
class RequestHandler : std::enable_shared_from_this<RequestHandler> {
public:
  explicit RequestHandler(int fd, std::shared_ptr<EpollManager> epoll)
      : fd_(fd)
      , epoll_(std::move(epoll)) {
    buffer_.resize(BUFFER_SIZE);
  }

  ~RequestHandler() {
    close(fd_);
  }

  void Handle(uint32_t events);

private:
  void HandleRead();

  void HandleWrite();

  static constexpr int BUFFER_SIZE = 4096;

  int fd_;
  std::string request_buffer_;
  std::string buffer_;

  std::string response_buffer_;
  bool finished_{false};

  std::shared_ptr<EpollManager> epoll_;
  std::shared_ptr<ProtocolHandler> protocol_handler_;
};


} // namespace net


#endif // HTTP_SERVER_SRC_REQUEST_HANDLER_H
