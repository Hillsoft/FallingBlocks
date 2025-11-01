#pragma once

namespace util {

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
};

} // namespace util
