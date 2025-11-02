#pragma once

#include <optional>
#include <stdexcept>
#include <string_view>
#include <variant>
#include "util/meta_utils.hpp"

namespace util {

template <typename... Args>
class TaggedVariant {
 public:
  template <typename... ConstructArgs>
  explicit TaggedVariant(ConstructArgs&&... args)
      : data_(std::forward<ConstructArgs>(args)...) {}

  TaggedVariant(const TaggedVariant& other) = default;
  TaggedVariant(TaggedVariant&& other) = default;

  template <typename Fn>
  static TaggedVariant visitConstruct(std::string_view tag, Fn&& fn) {
    std::optional<TaggedVariant> result;

    TArray<Args...>::visit([&](auto typeHolder) {
      if (!result.has_value() &&
          tag == decltype(typeHolder)::Value::First::value) {
        result.emplace(typename decltype(typeHolder)::Value::Second{
            fn(THolder<decltype(typeHolder)::Value::Second>{})});
      }
    });

    if (!result.has_value()) {
      throw std::runtime_error{"Tag not found"};
    }
    return *result;
  }

  template <typename Fn>
  auto visit(Fn&& fn) {
    return std::visit(std::forward<Fn>(fn), data_);
  }

 private:
  std::variant<typename Args::Second...> data_;
};

} // namespace util
