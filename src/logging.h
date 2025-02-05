//
// Created by binhtt4 on 05/02/2025.
//

#ifndef HTTP_SERVER_LOGGING_H
#define HTTP_SERVER_LOGGING_H


#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>


inline std::string Timestamp() {
  auto now = std::chrono::system_clock::now();
  auto epoch = now.time_since_epoch();
  auto sec = duration_cast<std::chrono::seconds>(epoch);
  auto ns = duration_cast<std::chrono::nanoseconds>(epoch - sec);

  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm now_tm = *std::localtime(&now_c);

  std::ostringstream oss;
  oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(9)
      << ns.count();

  return oss.str();
}


#define FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define LOG(level, msg)                                                                           \
  do {                                                                                            \
    Timestamp();                                                                                  \
    std::cout << Timestamp() << "  [" << (level) << "]\t[" << FILENAME << ":" << __LINE__ << "] " \
              << msg /*NOLINT*/ << std::endl;                                                     \
  } while (0)


// always define log error
#define LOG_ERROR(msg)                                                                \
  do {                                                                                \
    std::cerr << Timestamp() << "  [ERROR]\t[" << FILENAME << ":" << __LINE__ << "] " \
              << msg /*NOLINT*/ << std::endl;                                         \
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
