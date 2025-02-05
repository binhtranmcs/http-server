//
// Created by binhtranmcs on 05/02/2025.
//

#include "http_handler.h"


namespace net {


bool HttpHandler::Handle(HttpHandler::Request const& request, HttpHandler::Response& response) {
  try {
    auto handler_key = request.HandlerKey();
    auto iter = handlers_map_.find(handler_key);
    if (iter == handlers_map_.end()) {
      response = HttpResponse(404, "Not Found\n");
      return true;
    }
    // run handler callback
    return iter->second(request, response);
  } catch (...) {
    // for simplicity, catch all exceptions and return 503
    response = HttpResponse(503, "Internal Server Error\n");
    return true;
  }
}


bool HttpHandler::AddHandler(const std::string& path, HttpMethod method,
    HttpHandler::Handler&& handler) {
  std::string handler_key = ToString(method) + path;
  if (handlers_map_.find(handler_key) != handlers_map_.end()) {
    // path already exists
    return false;
  }
  handlers_map_.insert({handler_key, std::move(handler)});
  return true;
}


} // namespace net
