#pragma once

#include <string_view>

namespace blocks::log {

class LoggerBackend {
 public:
  LoggerBackend() = default;
  virtual ~LoggerBackend() = default;

  LoggerBackend(const LoggerBackend& other) = default;
  LoggerBackend& operator=(const LoggerBackend& other) = default;

  LoggerBackend(LoggerBackend&& other) = default;
  LoggerBackend& operator=(LoggerBackend&& other) = default;

  virtual void emitLine(std::string_view line) = 0;
};

} // namespace blocks::log
