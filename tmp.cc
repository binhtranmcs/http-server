#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024
#define NUM_THREADS 4

// Utility class for setting sockets to non-blocking mode
class Utils {
public:
  static void set_nonblocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  }
};

// ** Client Context: Stores request & response buffers **
struct ClientContext {
  int fd;
  std::string request_buffer;
  std::string response_buffer;
  bool writing = false;  // Track if we need to write

  explicit ClientContext(int client_fd) : fd(client_fd) {}
};

// ** HTTP Handler with Method & URI Handling **
class HttpHandler {
private:
  using HandlerFunction = std::function<std::string(ClientContext*, const std::string&)>;
  std::unordered_map<std::string, std::unordered_map<std::string, HandlerFunction>> method_uri_handlers;

public:
  void add_route(const std::string& method, const std::string& path, HandlerFunction handler) {
    method_uri_handlers[method][path] = handler;
  }

  std::string handle_request(ClientContext* client) {
    std::string method, uri, body;
    parse_request(client->request_buffer, method, uri, body);

    if (method_uri_handlers.find(method) != method_uri_handlers.end() &&
        method_uri_handlers[method].find(uri) != method_uri_handlers[method].end()) {
      return method_uri_handlers[method][uri](client, body);
    }
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 13\r\nConnection: close\r\n\r\n404 Not Found";
  }

  bool is_request_complete(ClientContext* client) {
    return client->request_buffer.find("\r\n\r\n") != std::string::npos;
  }

private:
  void parse_request(const std::string& request, std::string& method, std::string& uri, std::string& body) {
    size_t method_end = request.find(" ");
    size_t uri_end = request.find(" ", method_end + 1);
    method = request.substr(0, method_end);
    uri = request.substr(method_end + 1, uri_end - method_end - 1);

    size_t body_start = request.find("\r\n\r\n");
    if (body_start != std::string::npos) {
      body = request.substr(body_start + 4);
    }
  }
};

// ** Server Class: Manages epoll and connections **
class Server {
private:
  int server_fd, epoll_fd;
  struct epoll_event event, events[MAX_EVENTS];
  HttpHandler* protocol_handler;

public:
  explicit Server(HttpHandler* handler) : protocol_handler(handler) {}

  bool initialize(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, SOMAXCONN);

    Utils::set_nonblocking(server_fd);
    epoll_fd = epoll_create1(0);
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    return true;
  }

  void run() {
    std::vector<std::thread> workers;
    for (int i = 0; i < NUM_THREADS; i++) {
      workers.emplace_back(&Server::worker_thread, this);
    }

    for (auto& worker : workers) {
      worker.join();
    }
  }

private:
  void worker_thread() {
    struct epoll_event thread_events[MAX_EVENTS];

    while (true) {
      int event_count = epoll_wait(epoll_fd, thread_events, MAX_EVENTS, -1);
      for (int i = 0; i < event_count; i++) {
        if (thread_events[i].data.fd == server_fd) {
          accept_client();
        } else {
          ClientContext* client = static_cast<ClientContext*>(thread_events[i].data.ptr);
          handle_event(client, thread_events[i].events);
        }
      }
    }
  }

  void accept_client() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd >= 0) {
      Utils::set_nonblocking(client_fd);
      ClientContext* client = new ClientContext(client_fd);

      struct epoll_event event;
      event.events = EPOLLIN | EPOLLET;
      event.data.ptr = client;
      epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    }
  }

  void handle_event(ClientContext* client, uint32_t events) {
    if (events & EPOLLIN) {
      read_data(client);
    }
    if (events & EPOLLOUT) {
      send_data(client);
    }
  }

  void read_data(ClientContext* client) {
    char buffer[BUFFER_SIZE];

    while (true) {
      ssize_t bytes_read = read(client->fd, buffer, sizeof(buffer));

      if (bytes_read > 0) {
        client->request_buffer.append(buffer, bytes_read);

        if (protocol_handler->is_request_complete(client)) {
          client->response_buffer = protocol_handler->handle_request(client);
          send_data(client);
          return;
        }
      } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.ptr = client;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, &event);
        return;
      } else {
        close(client->fd);
        delete client;
        return;
      }
    }
  }

  void send_data(ClientContext* client) {
    while (!client->response_buffer.empty()) {
      ssize_t bytes_sent = send(client->fd, client->response_buffer.c_str(), client->response_buffer.size(), 0);

      if (bytes_sent > 0) {
        client->response_buffer.erase(0, bytes_sent);
      } else if (bytes_sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        struct epoll_event event;
        event.events = EPOLLOUT | EPOLLET;
        event.data.ptr = client;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, &event);
        return;
      } else {
        close(client->fd);
        delete client;
        return;
      }
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = client;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, &event);
  }
};


void parse_request(const std::string& request, std::string& method, std::string& uri,
                   std::unordered_map<std::string, std::string>& headers,
                   std::unordered_map<std::string, std::string>& query_params,
                   std::string& body) {
  size_t method_end = request.find(" ");
  size_t uri_end = request.find(" ", method_end + 1);
  size_t headers_end = request.find("\r\n\r\n");

  method = request.substr(0, method_end);
  uri = request.substr(method_end + 1, uri_end - method_end - 1);

  // Extract query parameters if present
  size_t query_start = uri.find("?");
  if (query_start != std::string::npos) {
    std::string query_string = uri.substr(query_start + 1);
    uri = uri.substr(0, query_start);
    parse_query_params(query_string, query_params);
  }

  // Extract headers
  size_t line_start = uri_end + 1;
  while (line_start < headers_end) {
    size_t line_end = request.find("\r\n", line_start);
    size_t colon_pos = request.find(": ", line_start);
    if (colon_pos != std::string::npos) {
      std::string header_name = request.substr(line_start, colon_pos - line_start);
      std::string header_value = request.substr(colon_pos + 2, line_end - colon_pos - 2);
      headers[header_name] = header_value;
    }
    line_start = line_end + 2;
  }

  // Extract body if Content-Length is specified
  if (headers.find("Content-Length") != headers.end()) {
    int content_length = std::stoi(headers["Content-Length"]);
    body = request.substr(headers_end + 4, content_length);
  }
}
void parse_query_params(const std::string& query, std::unordered_map<std::string, std::string>& params) {
  size_t start = 0;
  while (start < query.length()) {
    size_t eq = query.find('=', start);
    size_t amp = query.find('&', start);

    if (eq == std::string::npos) break;

    std::string key = query.substr(start, eq - start);
    std::string value = (amp == std::string::npos) ? query.substr(eq + 1) : query.substr(eq + 1, amp - eq - 1);

    params[key] = value;
    start = (amp == std::string::npos) ? query.length() : amp + 1;
  }
}
