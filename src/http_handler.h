//
// Created by binhtranmcs on 05/02/2025.
//

#ifndef HTTP_SERVER_SRC_HTTP_HANDLER_H_
#define HTTP_SERVER_SRC_HTTP_HANDLER_H_


#include <cassert>
#include <functional>
#include <sstream>
#include <string>

#include "http_method.h"
#include "http_request.h"
#include "http_response.h"


namespace net {


class HttpHandler {
public:
  using Handler = std::function<bool(const HttpRequest&, HttpResponse&)>;
  using Request = HttpRequest;
  using Response = HttpResponse;

  HttpHandler() = default;

  // return true if the request finished, false if more processing needed
  bool Handle(const Request& request_buffer, Response& response_buffer);

  bool AddHandler(const std::string& path, HttpMethod method, Handler&& handler);

private:
  std::unordered_map<std::string, Handler> handlers_map_;
};


} // namespace net


#endif // HTTP_SERVER_SRC_HTTP_HANDLER_H_
