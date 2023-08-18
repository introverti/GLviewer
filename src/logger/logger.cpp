#include <logger/logger.h>

#include <memory>

#include "spdlog/cfg/env.h"
namespace Common {
namespace Log {

Logger::Logger() {
  // async logger
  // spdlog::set_async_mode(32768);  // 2^15
  spdlog::init_thread_pool(8192, 1);
  std::vector<spdlog::sink_ptr> sinkList;

  // console sink
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_pattern("%Y-%m-%d %H:%M:%S [%l][%t] [%s:%#]<%!> %v");
  sinkList.push_back(consoleSink);

  // file sink
  const char* env_p = std::getenv("GL_LOG_DIR");
  if (env_p == NULL) env_p = std::getenv("HOME");
  if (env_p == NULL) env_p = "/tmp";
  // ensure the log path exists

  // get start-time for log file name
  time_t rawtime;
  struct tm* timeinfo;
  char buffer[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", timeinfo);
  std::string time_str(buffer);

  // log file path
  const std::string log_path = std::string(env_p) + "/gl_" + time_str + ".log";

  auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      log_path, 1024 * 1024 * 10, 10);
  rotating_sink->set_pattern("%Y-%m-%d %H:%M:%S [%l][%t] [%@]<%!> %v");
  sinkList.push_back(rotating_sink);

  // logger
  m_logger_ = std::make_shared<spdlog::async_logger>(
      "SL", sinkList.begin(), sinkList.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);
  // register it if you need to access it globally
  spdlog::register_logger(m_logger_);

  // load from env variable
  spdlog::cfg::load_env_levels();

  // flush level
  m_logger_->flush_on(spdlog::level::warn);
  spdlog::flush_every(std::chrono::seconds(2));

  // gloabal pattern
  // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
  // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%l][%t] [%s:%#]<%!> %v");
}

Logger::~Logger() { spdlog::shutdown(); }

}  // namespace Log
}  // namespace Common