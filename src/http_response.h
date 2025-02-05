//
// Created by binhtt4 on 09/02/25.
//

#ifndef HTTP_SERVER_SRC_HTTP_RESPONSE_H
#define HTTP_SERVER_SRC_HTTP_RESPONSE_H


namespace net {


class HttpResponse {
public:
  HttpResponse() = default;

  explicit HttpResponse(int code, std::string body = "")
      : status_code_(code)
      , body_(std::move(body)) {
    status_msg_ = HttpResponse::GetStatusMessage(status_code_);

    headers_["Content-Length"] = std::to_string(body_.size());
    headers_["Content-Type"] = "text/plain";
    headers_["Connection"] = "close";
  }

  std::string GetBuffer() {
    std::string response = "HTTP/1.1 " + std::to_string(status_code_) + " " + status_msg_ + "\r\n";
    for (const auto& [key, value] : headers_) {
      response += key;
      response += ": ";
      response += value;
      response += "\r\n";
    }
    response += "Connection: close\r\n\r\n";
    response += body_;
    return response;
  }

  void SetStatus(int status_code) {
    status_code_ = status_code;
  }

  void SetHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
  }

  void SetBody(std::string body) {
    body_ = std::move(body);
  }

private:
  static inline std::string GetStatusMessage(int status_code) {
    static std::unordered_map<int, std::string> status_map{
        {200, "OK"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {500, "Internal Server Error"},
        {503, "Service Unavailable"},
    };
    auto iter = status_map.find(status_code);
    return (iter == status_map.end()) ? "Unknown Error" : iter->second;
  }

  int status_code_{200};
  std::string status_msg_{"OK"};
  std::unordered_map<std::string, std::string> headers_;
  std::string body_;
};


} // namespace net


#endif // HTTP_SERVER_SRC_HTTP_RESPONSE_H
