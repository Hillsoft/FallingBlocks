#pragma once

#include <bit>
#include <cstring>
#include <numeric>
#include <string>
#include <type_traits>

namespace util {

namespace detail {

template <typename... TArgs>
struct StringConverter;

template <>
struct StringConverter<> {
  StringConverter() = default;
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  [[nodiscard]] size_t estimateSize() const { return 0; }
  void append(std::string& /* output */) const {}
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, const char*>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  template <typename... TConstructArgs>
  explicit StringConverter(const char* c, TConstructArgs&&... rest)
      // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
      : StringConverter<TArgs...>(std::forward<TConstructArgs>(rest)...),
        estimatedSizeCache_(strlen(c)) {}

  [[nodiscard]] size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, const char* c, TArgs&&... rest) const {
    output.append(std::string_view{c, estimatedSizeCache_});
    // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  size_t estimatedSizeCache_;
};

template <typename TArg, typename... TArgs>
// NOLINTNEXTLINE(*-avoid-c-arrays)
  requires std::is_same_v<std::remove_cvref_t<TArg>, char[sizeof(TArg)]>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  template <typename... TConstructArgs>
  // NOLINTNEXTLINE(*-avoid-c-arrays)
  explicit StringConverter(const char /*c*/[], TConstructArgs&&... rest)
      // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
      : StringConverter<TArgs...>(std::forward<TConstructArgs>(rest)...) {}

  [[nodiscard]] size_t estimateSize() const {
    return sizeof(TArg) - 1 + StringConverter<TArgs...>::estimateSize();
  }

  // NOLINTNEXTLINE(*-avoid-c-arrays)
  void append(std::string& output, const char c[], TArgs&&... rest) const {
    output.append(std::string_view{c, sizeof(TArg) - 1});
    // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, std::string_view>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  template <typename... TConstructArgs>
  explicit StringConverter(std::string_view c, TConstructArgs&&... rest)
      // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
      : StringConverter<TArgs...>(std::forward<TConstructArgs>(rest)...),
        estimatedSizeCache_(c.size()) {}

  [[nodiscard]] size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, std::string_view c, TArgs&&... rest) const {
    output.append(c);
    // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  size_t estimatedSizeCache_;
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, std::string>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  template <typename... TConstructArgs>
  // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
  explicit StringConverter(const std::string& c, TConstructArgs&&... rest)
      : StringConverter<TArgs...>(std::forward<TConstructArgs>(rest)...),
        estimatedSizeCache_(c.size()) {}

  [[nodiscard]] size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(
      std::string& output, const std::string& c, TArgs&&... rest) const {
    output.append(c);
    // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  size_t estimatedSizeCache_;
};

template <typename TArg>
  requires std::is_integral_v<TArg>
static size_t intStringSizeEstimate(TArg a) {
  size_t estimate = 1;
  size_t absA = 0;
  if constexpr (std::is_signed_v<TArg>) {
    if (a < 0) {
      estimate++;

      absA = -a;
    } else {
      absA = a;
    }
  } else {
    absA = a;
  }

  const size_t bits = (8 * sizeof(size_t)) - std::countl_zero(absA);
  estimate += bits / 3;
  return estimate;
}

template <typename TArg, typename... TArgs>
  requires std::is_integral_v<std::remove_cvref_t<TArg>>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  template <typename... TConstructArgs>
  explicit StringConverter(
      const std::remove_cvref_t<TArg>& a, TConstructArgs&&... rest)
      // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
      : StringConverter<TArgs...>(std::forward<TConstructArgs>(rest)...),
        estimatedSizeCache_(intStringSizeEstimate(a)) {}

  [[nodiscard]] size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, const TArg& a, TArgs&&... rest) const {
    output.append(std::to_string(a));
    // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  size_t estimatedSizeCache_;
};

} // namespace detail

template <typename... TArgs>
std::string toString(TArgs&&... args) {
  // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
  const detail::StringConverter<TArgs...> convertor{args...};
  const size_t estimatedSize = convertor.estimateSize();
  std::string output;
  output.reserve(estimatedSize);
  // NOLINTNEXTLINE(*-array-to-pointer-decay,hicpp-no-array-decay)
  convertor.append(output, std::forward<TArgs>(args)...);
  return output;
}

} // namespace util
