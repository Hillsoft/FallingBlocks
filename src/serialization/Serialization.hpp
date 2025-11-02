#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "util/string.hpp"

namespace blocks::serialization {

template <typename T>
struct OptionalTraits {
  static constexpr bool isOptional = false;
  using UnderlyingType = T;
};
template <typename T>
struct OptionalTraits<std::optional<T>> {
  static constexpr bool isOptional = true;
  using UnderlyingType = T;
};

template <typename T>
concept SerializableStruct = requires(T obj) { typename T::Fields; };

template <typename T>
struct deserializeArbitrary;

template <typename TField, typename TCursor>
TField::Second deserializeField(TCursor cursor, size_t& availableFieldCount) {
  std::optional<TCursor> subFieldCursor =
      cursor.getSubFieldCursor(TField::First::value);

  if (subFieldCursor.has_value()) {
    availableFieldCount--;
    return deserializeArbitrary<
        typename OptionalTraits<typename TField::Second>::UnderlyingType>{}(
        *subFieldCursor);
  } else {
    if constexpr (OptionalTraits<typename TField::Second>::isOptional) {
      return std::nullopt;
    } else {
    throw std::runtime_error{
        util::toString("Field not found: ", TField::First::value)};
  }
}
}

template <typename T>
  requires(SerializableStruct<T>)
struct deserializeArbitrary<T> {
  template <typename TCursor>
  T operator()(TCursor cursor) {
    size_t availableFieldCount = cursor.getStructFieldCount();

    T result = T::Fields::template visitConstruct<T>([&](auto tholder) {
      return deserializeField<typename decltype(tholder)::Value>(
          cursor, availableFieldCount);
    });

    if (availableFieldCount != 0) {
      throw std::runtime_error{"Unrecognised fields"};
    }

    return result;
  }
};

template <typename T>
struct deserializeArbitrary<std::unordered_map<std::string, T>> {
  template <typename TCursor>
  std::unordered_map<std::string, T> operator()(TCursor cursor) {
    std::unordered_map<std::string, T> result;
    result.reserve(cursor.getStructFieldCount());

    for (auto it = cursor.getFieldStartIterator();
         it != cursor.getFieldEndIterator();
         ++it) {
      result.emplace(
          std::string{it.fieldName()},
          deserializeArbitrary<T>{}(it.fieldCursor()));
    }

    return result;
  }
};

template <typename T>
struct deserializeArbitrary<std::vector<T>> {
  template <typename TCursor>
  std::vector<T> operator()(TCursor cursor) {
    std::vector<T> result;
    result.reserve(cursor.getSequenceEntryCount());

    for (auto it = cursor.getSequenceStartIterator();
         it != cursor.getSequenceEndIterator();
         ++it) {
      result.emplace_back(deserializeArbitrary<T>{}(it.fieldCursor()));
    }

    return result;
  }
};

template <>
struct deserializeArbitrary<std::string> {
  template <typename TCursor>
  std::string operator()(TCursor cursor) {
    return std::string{cursor.getStringValue()};
  }
};

template <typename T, typename SerializationProvider>
T deserialize(std::string_view dataStream) {
  SerializationProvider deserializer{dataStream};

  typename SerializationProvider::TCursor rootCursor =
      deserializer.getRootCursor();

  return deserializeArbitrary<T>{}(rootCursor);
}

} // namespace blocks::serialization
