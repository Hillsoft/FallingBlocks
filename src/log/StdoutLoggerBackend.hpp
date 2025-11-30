#pragma once

#include <string_view>
#include "log/LoggerBackend.hpp"

namespace blocks::log {

class StdoutLoggerBackend : public LoggerBackend {
 public:
  StdoutLoggerBackend() = default;

  void emitLine(std::string_view message) final;
};

} // namespace blocks::log
