#pragma once

#include <string_view>

namespace util {

template <size_t i, typename Arg, typename... Args>
struct ElementAtImpl {
  using Value = ElementAtImpl<i - 1, Args...>::Value;
};

template <typename Arg, typename... Args>
struct ElementAtImpl<0, Arg, Args...> {
  using Value = Arg;
};

template <size_t i, typename... Args>
using ElementAt = ElementAtImpl<i, Args...>::Value;

template <unsigned N>
struct FixedString {
  char buf[N + 1]{};
  constexpr FixedString(char const* s) {
    for (unsigned i = 0; i != N; ++i)
      buf[i] = s[i];
  }
  constexpr operator char const*() const { return buf; }
};
template <unsigned N>
FixedString(char const (&)[N]) -> FixedString<N - 1>;

template <FixedString TValue>
struct TString {
  static constexpr std::string_view value{TValue};
};

template <typename T1, typename T2>
struct TPair {
  using First = T1;
  using Second = T2;
};

template <typename T>
struct THolder {
  using Value = T;
};

template <typename... TArr>
struct TArray {
  template <typename ConstructType, typename ValueFn>
  static auto visitConstruct(ValueFn&& valueFn) {
    return ConstructType{valueFn(THolder<TArr>{})...};
  }

  template <typename Fn>
  static void visit(Fn&& fn) {
    (fn(THolder<TArr>{}), ...);
  }
};

} // namespace util
