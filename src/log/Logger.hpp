#pragma once

#include <cstdint>
#include <memory>
#include <source_location>
#include <string_view>
#include <vector>
#include "log/LoggerBackend.hpp"

namespace blocks::log {

enum class LogLevel : uint8_t { INFO, WARNING, ERR };

class LoggerSystem {
 public:
  LoggerSystem() = default;

  void registerBackend(std::unique_ptr<LoggerBackend> backend);

  void log(
      LogLevel level,
      std::string_view message,
      std::source_location location = std::source_location::current());

  static void setDefaultLoggerSystem(LoggerSystem* system);
  static void logToDefault(
      LogLevel level,
      std::string_view message,
      std::source_location location = std::source_location::current());

 protected:
  std::vector<std::unique_ptr<LoggerBackend>> backends_;
};

} // namespace blocks::log
