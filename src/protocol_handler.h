//
// Created by binhtt4 on 04/02/25.
//

#ifndef HTTP_SERVER_SRC_PROTOCOL_HANDLER_H
#define HTTP_SERVER_SRC_PROTOCOL_HANDLER_H


#include <iostream>
#include <string>

#include "logging.h"


namespace net {


template <typename T>
concept IProtocolHandler = requires(T t, T::Request request, T::Response response,
    std::string& buffer, const T::Request& req, T::Response& res) {
  typename T::Request;
  typename T::Response;

  // return true if the request is parsed to process further, false if it still needs more data
  { request.ParseBuffer(buffer) } -> std::same_as<bool>;

  // convert response data to std::string to send back to client
  { response.GetBuffer() } -> std::convertible_to<std::string>;

  // return true if the request finished, false otherwise
  { t.Handle(req, res) } -> std::same_as<bool>;
  // { t.AddHandler(callback) };
};


class EchoHandler {
public:
  struct Request {
    std::string buffer_;
    bool ParseBuffer(const std::string& buffer) {
      buffer_ = buffer;
      return true;
    }
  };

  struct Response {
    std::string buffer_;
    [[nodiscard]] std::string GetBuffer() const {
      return buffer_;
    }
  };

  EchoHandler() = default;

  bool Handle(const Request& request, Response& response) const {
    (void)dummy;
    LOG_DEBUG(request.buffer_);
    response.buffer_ = "hello " + request.buffer_;
    return true;
  }

private:
  long long dummy;
};


} // namespace net


#endif // HTTP_SERVER_SRC_PROTOCOL_HANDLER_H
