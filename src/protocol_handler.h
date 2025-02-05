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
concept IProtocolHandler = requires(T handler, const std::string& req, std::string& res) {
  { handler.ParseBuffer(req) } -> std::convertible_to<std::string>;
  { handler.Handle(req, res) } -> std::same_as<bool>;
};


class EchoHandler {
public:
  EchoHandler() = default;

  [[nodiscard]] std::string ParseBuffer(const std::string& buffer) const {
    (void *)dummy;
    return buffer;
  }

  bool Handle(const std::string& request_buffer, std::string& response_buffer) const {
    (void *)dummy;
    LOG_DEBUG(request_buffer);
    response_buffer = "hello " + request_buffer;
    return true;
  }

private:
  long long dummy;
};


} // namespace net


#endif // HTTP_SERVER_SRC_PROTOCOL_HANDLER_H
