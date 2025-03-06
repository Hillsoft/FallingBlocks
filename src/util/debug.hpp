#pragma once

#ifdef NDEBUG
#define DEBUG_ASSERT(condition)
#else
#define DEBUG_ASSERT(condition) \
  util::debugAssert((condition), #condition, std::source_location::current())

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string_view>

namespace util {

void debugAssert(
    bool condition,
    std::string_view conditionText,
    std::source_location location) {
  if (!condition) {
    std::cout << "Assert failed!\n"
              << conditionText << "\nAt: " << location.file_name() << "("
              << location.line() << ":" << location.column() << ")\n`"
              << location.function_name() << "`" << std::endl;
    std::abort();
  }
}

} // namespace util

#endif
