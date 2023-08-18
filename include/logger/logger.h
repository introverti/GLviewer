#pragma once
#include <string>

#ifndef ABORT
#define ABORT() abort()
#endif

#include "spdlog/async.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace Common {
namespace Log {
class Logger {
 public:
  auto GetLogger() { return m_logger_; }

  static Logger& GetInstance() {
    static Logger m_instance;
    return m_instance;
  }

 private:
  Logger();
  ~Logger();
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  std::shared_ptr<spdlog::logger> m_logger_;
};
}  // namespace Log
}  // namespace Common

#define SPDLOG_LOGGER_CALL_(level, ...)                                 \
  do {                                                                  \
    Common::Log::Logger::GetInstance().GetLogger()->log(                \
        spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, \
        __VA_ARGS__);                                                   \
  } while (0)

// (TODO: Xavier) SPDLOG seems to have a bug in the trace level, so I disable it

#define log_debug(...) SPDLOG_LOGGER_CALL_(spdlog::level::debug, __VA_ARGS__)

#define log_info(...) SPDLOG_LOGGER_CALL_(spdlog::level::info, __VA_ARGS__)

#define log_warning(...) SPDLOG_LOGGER_CALL_(spdlog::level::warn, __VA_ARGS__)

#define log_error(...) SPDLOG_LOGGER_CALL_(spdlog::level::err, __VA_ARGS__)

#define log_critical(...) \
  SPDLOG_LOGGER_CALL_(spdlog::level::critical, __VA_ARGS__)

#define log_fatal(...)                                    \
  do {                                                    \
    SPDLOG_LOGGER_CALL_(spdlog::level::off, __VA_ARGS__); \
    ABORT();                                              \
  } while (0)

#define log_verify(condition, ...)                          \
  do {                                                      \
    if (!(condition)) {                                     \
      SPDLOG_LOGGER_CALL_(spdlog::level::off, __VA_ARGS__); \
    }                                                       \
  } while (0)

#define log_assert(condition, ...)                          \
  do {                                                      \
    if (!(condition)) {                                     \
      SPDLOG_LOGGER_CALL_(spdlog::level::off, __VA_ARGS__); \
      ABORT();                                              \
    }                                                       \
  } while (0)
