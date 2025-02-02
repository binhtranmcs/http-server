//
// Created by binhtranmcs on 01/02/2025.
//

#ifndef HTTP_SERVER_SRC_UTILS_H_
#define HTTP_SERVER_SRC_UTILS_H_


#include <fcntl.h>


namespace utils {


inline void SetNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


} // namespace utils


#endif // HTTP_SERVER_SRC_UTILS_H_
