#include "log/Logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "log/LoggerBackend.hpp"
#include "util/string.hpp"

namespace blocks::log {

namespace {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
LoggerSystem* defaultLoggerSystem = nullptr;

std::string_view getLogLevelString(LogLevel level) {
  switch (level) {
    case LogLevel::INFO:
      return "[INFO]    ";
    case LogLevel::WARNING:
      return "[WARNING] ";
    case LogLevel::ERR:
      return "[ERROR]   ";
  }
  return "[UNKNOWN] ";
}

std::string getCurrentTimeString() {
  const std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  tm buf{};
  if (localtime_s(&buf, &now) != 0) {
    return "0000-00-00 00:00:00";
  }
  std::stringstream timeStr;
  timeStr << std::put_time(&buf, "%F %T");
  return std::move(timeStr).str();
}

constexpr std::string_view getShortFileName(std::string_view longFileName) {
  size_t pos = longFileName.find_last_of('/');
  if (pos == std::string_view::npos) {
    pos = longFileName.find_last_of('\\');
  }
  if (pos == std::string_view::npos) {
    return longFileName;
  }
  return longFileName.substr(1 + pos);
}

} // namespace

void LoggerSystem::registerBackend(std::unique_ptr<LoggerBackend> backend) {
  backends_.emplace_back(std::move(backend));
}

void LoggerSystem::log(
    LogLevel level, std::string_view message, std::source_location location) {
  const std::string fullMessage = util::toString(
      getCurrentTimeString(),
      " ",
      getLogLevelString(level),
      getShortFileName(location.file_name()),
      " (",
      location.line(),
      "): ",
      message,
      "\n");

  for (auto& backend : backends_) {
    backend->emitLine(fullMessage);
  }
}

void LoggerSystem::setDefaultLoggerSystem(LoggerSystem* system) {
  defaultLoggerSystem = system;
}

void LoggerSystem::logToDefault(
    LogLevel level, std::string_view message, std::source_location location) {
  if (defaultLoggerSystem != nullptr) {
    defaultLoggerSystem->log(level, message, location);
  }
}

} // namespace blocks::log
