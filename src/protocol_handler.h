//
// Created by binhtt4 on 04/02/25.
//

#ifndef HTTP_SERVER_SRC_PROTOCOL_HANDLER_H
#define HTTP_SERVER_SRC_PROTOCOL_HANDLER_H


#include <iostream>
#include <string>


namespace net {


// TODO make a concept of this and implement HttpHandler
class ProtocolHandler {
public:
  ProtocolHandler() {
  }

  std::string ParseBuffer(const std::string& buffer) {
    return buffer;
  }

  bool Handle(const std::string& request_buffer, std::string& response_buffer) {
    std::cerr << request_buffer << std::endl;
    response_buffer = "hello " + request_buffer;
    return true;
  }

private:
};


} // namespace net


#endif // HTTP_SERVER_SRC_PROTOCOL_HANDLER_H
