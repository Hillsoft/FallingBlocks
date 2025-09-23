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
  size_t estimateSize() const { return 0; }
  void append(std::string& /* output */) {}
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, const char*>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  StringConverter(const char* c, TArgs&&... rest)
      : StringConverter<TArgs...>(std::forward<TArgs>(rest)...),
        estimatedSizeCache_(strlen(c)) {}

  size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, const char* c, TArgs&&... rest) {
    output.append(std::string_view{c, estimatedSizeCache_});
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  const size_t estimatedSizeCache_;
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, char[sizeof(TArg)]>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  StringConverter(const char /*c*/[], TArgs&&... rest)
      : StringConverter<TArgs...>(std::forward<TArgs>(rest)...) {}

  size_t estimateSize() const {
    return sizeof(TArg) - 1 + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, const char c[], TArgs&&... rest) {
    output.append(std::string_view{c, sizeof(TArg) - 1});
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, std::string_view>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  StringConverter(std::string_view c, TArgs&&... rest)
      : StringConverter<TArgs...>(std::forward<TArgs>(rest)...),
        estimatedSizeCache_(c.size()) {}

  size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, std::string_view c, TArgs&&... rest) {
    output.append(c);
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  const size_t estimatedSizeCache_;
};

template <typename TArg, typename... TArgs>
  requires std::is_same_v<std::remove_cvref_t<TArg>, std::string>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  StringConverter(const std::string& c, TArgs&&... rest)
      : StringConverter<TArgs...>(std::forward<TArgs>(rest)...),
        estimatedSizeCache_(c.size()) {}

  size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, const std::string& c, TArgs&&... rest) {
    output.append(c);
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  const size_t estimatedSizeCache_;
};

template <typename TArg>
  requires std::is_integral_v<TArg>
static size_t intStringSizeEstimate(TArg a) {
  size_t estimate = 1;
  size_t absA;
  if (a < 0) {
    estimate++;

    absA = -a;
  } else {
    absA = a;
  }

  size_t bits = 8 * sizeof(size_t) - std::countl_zero(absA);
  estimate += bits / 3;
  return estimate;
}

template <typename TArg, typename... TArgs>
  requires std::is_integral_v<std::remove_cvref_t<TArg>>
struct StringConverter<TArg, TArgs...> : public StringConverter<TArgs...> {
  StringConverter(const TArg& a, TArgs&&... rest)
      : StringConverter<TArgs...>(std::forward<TArgs>(rest)...),
        estimatedSizeCache_(intStringSizeEstimate(a)) {}

  size_t estimateSize() const {
    return estimatedSizeCache_ + StringConverter<TArgs...>::estimateSize();
  }

  void append(std::string& output, const TArg& a, TArgs&&... rest) {
    output.append(std::to_string(a));
    StringConverter<TArgs...>::append(output, std::forward<TArgs>(rest)...);
  }

  const size_t estimatedSizeCache_;
};

} // namespace detail

template <typename... TArgs>
std::string toString(TArgs&&... args) {
  detail::StringConverter<TArgs...> convertor{std::forward<TArgs>(args)...};
  size_t estimatedSize = convertor.estimateSize();
  std::string output;
  output.reserve(estimatedSize);
  convertor.append(output, std::forward<TArgs>(args)...);
  return output;
}

} // namespace util
