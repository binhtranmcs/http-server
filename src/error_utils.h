//
// Created by binhtranmcs on 01/02/2025.
//

#ifndef HTTP_SERVER_SRC_ERROR_UTILS_H_
#define HTTP_SERVER_SRC_ERROR_UTILS_H_


#include <cstring>
#include <iostream>
#include <stdexcept>

#include "logging.h"


#define THROW_IF_ERROR_LT_0(error, error_msg)                                \
  do {                                                                       \
    if ((error) < 0) {                                                       \
      LOG_ERROR(error_msg /*NOLINT*/ << ", errno " << std::strerror(errno)); \
      throw std::runtime_error(error_msg);                                   \
    }                                                                        \
  } while (0)


#endif // HTTP_SERVER_SRC_ERROR_UTILS_H_
