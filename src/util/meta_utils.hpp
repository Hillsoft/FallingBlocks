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
  // NOLINTNEXTLINE(*-avoid-c-arrays)
  char buf[N + 1]{};
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr FixedString(char const* s) {
    for (unsigned i = 0; i != N; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-pointer-arithmetic)
      buf[i] = s[i];
    }
  }
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator char const*() const { return buf; }
};
template <unsigned N>
// NOLINTNEXTLINE(*-avoid-c-arrays)
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
  constexpr static size_t size = sizeof...(TArr);

  template <typename ConstructType, typename ValueFn>
  static auto visitConstruct(ValueFn&& valueFn) {
    return ConstructType{std::forward<ValueFn>(valueFn)(THolder<TArr>{})...};
  }

  template <typename Fn>
  static void visit(Fn&& fn) {
    (std::forward<Fn>(fn)(THolder<TArr>{}), ...);
  }

  template <typename... TArr2>
  using Append = TArray<TArr..., TArr2...>;
};

namespace detail {

template <typename T>
constexpr std::string_view getRawTypeName() {
#ifdef _MSC_VER
  return std::string_view{__FUNCSIG__};
#else
  static_assert(false, "getTypeName unsupported for this compiler");
#endif
}

struct TypeNameJunk {
  std::size_t leading = 0;
  std::size_t total = 0;
};
constexpr TypeNameJunk typeJunkDetails = []() {
  const std::string_view rawIntName = getRawTypeName<int>();
  auto pos = rawIntName.find("int");
  return TypeNameJunk{.leading = pos, .total = rawIntName.size() - 3};
}();

template <typename T>
constexpr std::string_view getTypeName() {
  const std::string_view rawTypeName = getRawTypeName<T>();
  std::string_view fullTypeName = rawTypeName.substr(
      typeJunkDetails.leading, rawTypeName.size() - typeJunkDetails.total);
  size_t namespaceSepPos = 0;
  while (
      (namespaceSepPos = fullTypeName.find("::")) != std::string_view::npos) {
    fullTypeName = fullTypeName.substr(namespaceSepPos + 2);
  }
  return fullTypeName;
}

} // namespace detail

template <typename T>
constexpr std::string_view typeName = detail::getTypeName<T>();

} // namespace util
