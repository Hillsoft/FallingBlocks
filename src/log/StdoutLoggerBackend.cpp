#include "log/StdoutLoggerBackend.hpp"

#include <iostream>
#include <string_view>

namespace blocks::log {

void StdoutLoggerBackend::emitLine(std::string_view message) {
  std::cout << message;
}

} // namespace blocks::log
