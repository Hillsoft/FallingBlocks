#pragma once

#include <array>

namespace util {

template <typename TArrType, typename... TArgs>
constexpr std::array<TArrType, sizeof...(TArgs)> makeArray(TArgs&&... args) {
  return std::array<TArrType, sizeof...(TArgs)>{std::forward<TArgs>(args)...};
}

} // namespace util
