#pragma once

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
