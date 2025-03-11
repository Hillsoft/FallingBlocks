#pragma once

#include <type_traits>

namespace util {

namespace detail {

template <typename TVal>
TVal sumImpl() {
  return 0;
}

template <typename TVal, typename... TVals>
TVal sumImpl(TVal&& val, TVals&&... vals) {
  return val + sumImpl<TVal>(std::forward<TVals>(vals)...);
}

} // namespace detail

template <typename... TArgs, typename TVal = std::common_type_t<TArgs...>>
  requires(std::is_arithmetic_v<TVal>)
TVal sum(TArgs&&... args) {
  return detail::sumImpl<TVal>(std::forward<TArgs>(args)...);
}

} // namespace util
