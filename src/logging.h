//
// Created by binhtt4 on 05/02/2025.
//

#ifndef HTTP_SERVER_LOGGING_H
#define HTTP_SERVER_LOGGING_H


#include <cstring>


#define FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define LOG(level, msg)                                                         \
  do {                                                                          \
    std::cout << "[" << (level) << "] [" << FILENAME << ":" << __LINE__ << "] " \
              << msg /*NOLINT*/ << std::endl;                                   \
  } while (0)


// always define log error
#define LOG_ERROR(msg)                                              \
  do {                                                              \
    std::cerr << "[ERROR] [" << FILENAME << ":" << __LINE__ << "] " \
              << msg /*NOLINT*/ << std::endl;                       \
  } while (0)

#if defined(LOG_LEVEL_DEBUG)
#define LOG_DEBUG(msg) LOG("DEBUG", msg)
#define LOG_INFO(msg) LOG("INFO", msg)
#define LOG_WARN(msg) LOG("WARN", msg)
#elif defined(LOG_LEVEL_INFO)
#define LOG_DEBUG(msg)
#define LOG_INFO(msg) LOG("INFO", msg)
#define LOG_WARN(msg) LOG("WARN", msg)
#elif defined(LOG_LEVEL_WARN)
#define LOG_DEBUG(msg)
#define LOG_INFO(msg)
#define LOG_WARN(msg) LOG("WARN", msg)
#else
#define LOG_DEBUG(msg)
#define LOG_INFO(msg)
#define LOG_WARN(msg)
#endif


#endif // HTTP_SERVER_LOGGING_H
