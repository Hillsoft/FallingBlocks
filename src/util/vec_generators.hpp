#pragma once

#include <span>
#include "util/Generator.hpp"

namespace util::vec {

template <typename T, typename Pred>
Generator<std::span<T>> genGroups(std::span<T> data, Pred p) {
  size_t groupStart = 0;
  for (size_t i = 1; i < data.size(); i++) {
    if (!p(data[groupStart], data[i])) {
      co_yield std::span{data.begin() + groupStart, data.begin() + i};
      groupStart = i;
    }
  }

  co_yield std::span{data.begin() + groupStart, data.end()};
}

template <typename T>
Generator<std::span<T>> genGroups(std::span<T> data) {
  return genGroups(data, [](const auto& a, const auto& b) { return a == b; });
}

} // namespace util::vec
