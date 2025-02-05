//
// Created by binhtt4 on 09/02/25.
//

#ifndef HTTP_SERVER_SRC_HTTP_METHOD_H
#define HTTP_SERVER_SRC_HTTP_METHOD_H


#include <string>


namespace net {


enum class HttpMethod {
  GET,
  POST,
};


inline std::string ToString(HttpMethod method) {
  switch (method) {
    case HttpMethod::GET:
      return "GET";
    case HttpMethod::POST:
      return "POST";
    default:
      return "UNSUPPORTED";
  }
}


} // namespace net


#endif // HTTP_SERVER_SRC_HTTP_METHOD_H
