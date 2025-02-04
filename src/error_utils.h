//
// Created by binhtranmcs on 01/02/2025.
//

#ifndef HTTP_SERVER_SRC_ERROR_UTILS_H_
#define HTTP_SERVER_SRC_ERROR_UTILS_H_


#include <cstring>
#include <iostream>
#include <stdexcept>


#define THROW_IF_ERROR_LT_0(error, error_msg)                               \
  do {                                                                      \
    if ((error) < 0) {                                                      \
      std::cerr << error_msg << ", errno " << std::strerror(errno) << '\n'; \
      throw std::runtime_error(error_msg);                                  \
    }                                                                       \
  } while (0)


#endif // HTTP_SERVER_SRC_ERROR_UTILS_H_
