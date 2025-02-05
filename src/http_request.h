//
// Created by binhtt4 on 09/02/25.
//

#ifndef HTTP_SERVER_SRC_HTTP_REQUEST_H
#define HTTP_SERVER_SRC_HTTP_REQUEST_H


#include <cassert>
#include <string>
#include <unordered_map>

#include "logging.h"


namespace net {


class HttpRequest {
public:
  bool ParseBuffer(const std::string& buffer) {
    if (!buffer_parsed_) {
      header_size_ = buffer.find("\r\n\r\n");
      if (header_size_ == std::string::npos) {
        // header not complete
        return false;
      }
      std::string_view header(buffer.data(), header_size_ + 4);
      ParseHeader(header);
      buffer_parsed_ = true;
    }
    if (content_length_ == -1) {
      // request does not have body
      return true;
    }
    if (buffer.size() < header_size_ + content_length_) {
      // body not fully received
      return false;
    }
    body_ = buffer.substr(header_size_ + 4, content_length_);
    return true;
  }

  void ParseHeader(std::string_view header_buffer) {
    size_t line_end = header_buffer.find("\r\n");
    assert(line_end != std::string_view::npos);

    // parse request line: "GET /index.html HTTP/1.1"
    std::string_view request_line = header_buffer.substr(0, line_end);
    size_t method_end = request_line.find(' ');
    if (method_end == std::string_view::npos) return;

    size_t path_end = request_line.find(' ', method_end + 1);
    if (path_end == std::string_view::npos) return;

    method_ = request_line.substr(0, method_end);
    path_ = request_line.substr(method_end + 1, path_end - method_end - 1);
    version_ = request_line.substr(path_end + 1);

    // parse headers
    auto pos = line_end + 2;
    while (pos < header_buffer.size()) {
      size_t next_line = header_buffer.find("\r\n", pos);
      assert(next_line != std::string_view::npos);

      std::string_view line = header_buffer.substr(pos, next_line - pos);
      pos = next_line + 2;

      if (line.empty()) break; // end of header

      size_t colon_pos = line.find(':');
      if (colon_pos == std::string_view::npos) continue;

      auto key = std::string(line.substr(0, colon_pos));
      auto value = std::string(line.substr(colon_pos + 1));
      headers_[key] = value;
    }
    handler_key_ = method_ + path_;
    auto content_length_iter = headers_.find("Content-Length");
    if (content_length_iter != headers_.end()) {
      content_length_ = std::stoi(content_length_iter->second);
    }

    LOG_DEBUG(handler_key_ << ' ' << content_length_);
  }

  std::string HandlerKey() const {
    return handler_key_;
  }

  std::string Method() const {
    return method_;
  }

  std::string Path() const {
    return path_;
  }

  std::string Body() const {
    return body_;
  }

private:
  bool buffer_parsed_{false};
  size_t header_size_{};

  std::string method_{};
  std::string path_{};
  std::string handler_key_{};

  std::string version_{};
  std::unordered_map<std::string, std::string> headers_{};

  int content_length_{-1};
  std::string body_{};
};


} // namespace net


#endif // HTTP_SERVER_SRC_HTTP_REQUEST_H
