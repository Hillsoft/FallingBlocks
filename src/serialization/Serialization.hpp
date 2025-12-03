#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "util/TaggedVariant.hpp"
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

template <>
struct deserializeArbitrary<std::string> {
  template <typename TCursor>
  std::string operator()(TCursor cursor) {
    return std::string{cursor.getStringValue()};
  }
};

template <>
struct deserializeArbitrary<int> {
  template <typename TCursor>
  int operator()(TCursor cursor) {
    return std::stoi(std::string{cursor.getStringValue()});
  }
};

template <>
struct deserializeArbitrary<unsigned long long> {
  template <typename TCursor>
  unsigned long long operator()(TCursor cursor) {
    return std::stoull(std::string{cursor.getStringValue()});
  }
};

template <>
struct deserializeArbitrary<float> {
  template <typename TCursor>
  float operator()(TCursor cursor) {
    return std::stof(std::string{cursor.getStringValue()});
  }
};

template <typename T>
  requires(SerializableStruct<T>)
struct deserializeArbitrary<T> {
  template <typename TCursor>
  T operator()(TCursor cursor) {
    if constexpr (T::Fields::size == 0) {
      if (!cursor.getStringValue().empty()) {
        throw std::runtime_error{"Unexpected contents in empty struct"};
      }
      return {};
    }

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

template <typename... Args>
struct deserializeArbitrary<util::TaggedVariant<Args...>> {
  template <typename TCursor>
  util::TaggedVariant<Args...> operator()(TCursor cursor) {
    const size_t fieldCount = cursor.getStructFieldCount();
    if (fieldCount != 2) {
      throw std::runtime_error{"Unrecognised fields"};
    }

    std::optional<TCursor> tagNameCursor =
        cursor.getSubFieldCursor("variantTag");
    if (!tagNameCursor.has_value()) {
      throw std::runtime_error{"Variant missing 'variantTag' field"};
    }
    const std::string tag = deserializeArbitrary<std::string>{}(*tagNameCursor);

    std::optional<TCursor> contentsCursor = cursor.getSubFieldCursor("value");
    if (!contentsCursor.has_value()) {
      throw std::runtime_error{"Variant missing 'value' field"};
    }

    return util::TaggedVariant<Args...>::visitConstruct(tag, [&](auto tholder) {
      return deserializeArbitrary<typename decltype(tholder)::Value>{}(
          *contentsCursor);
    });
  }
};

template <typename T, typename SerializationProvider>
T deserialize(std::string_view dataStream) {
  SerializationProvider deserializer{dataStream};

  const typename SerializationProvider::TCursor rootCursor =
      deserializer.getRootCursor();

  return deserializeArbitrary<T>{}(rootCursor);
}

template <typename T>
struct serializeArbitrary;

template <>
struct serializeArbitrary<std::string> {
  template <typename SerializationProvider>
  void operator()(
      SerializationProvider& /* provider */,
      SerializationProvider::TObject& curObject,
      const std::string& value) {
    curObject.setLeafValue(value);
  }
};

template <typename TIntegral>
  requires(std::is_integral_v<TIntegral>)
struct serializeArbitrary<TIntegral> {
  template <typename SerializationProvider>
  void operator()(
      SerializationProvider& /* provider */,
      SerializationProvider::TObject& curObject,
      const TIntegral& value) {
    curObject.setLeafValue(util::toString(value));
  }
};

template <>
struct serializeArbitrary<float> {
  template <typename SerializationProvider>
  void operator()(
      SerializationProvider& /* provider */,
      SerializationProvider::TObject& curObject,
      float value) {
    curObject.setLeafValue(util::toString(value));
  }
};

template <typename T>
  requires(SerializableStruct<T>)
struct serializeArbitrary<T> {
  template <typename SerializationProvider>
  void operator()(
      SerializationProvider& provider,
      SerializationProvider::TObject& curObject,
      const T& value) {
    static_assert(T::Fields::size > 0);
    T::Fields::visitIndexed([&](auto index, auto tholder) {
      typename SerializationProvider::TObject fieldValueObject =
          provider.makeSubObject();
      // Recurse
      serializeArbitrary<typename decltype(tholder)::Value::Second>{}(
          provider,
          fieldValueObject,
          value.template get<decltype(index)::value>());

      curObject.pushMappingEntry(
          std::string{decltype(tholder)::Value::First::value},
          std::move(fieldValueObject));
    });
  }
};

template <typename T, typename SerializationProvider>
std::string serialize(const T& data) {
  SerializationProvider serializer{};

  typename SerializationProvider::TObject& rootObject =
      serializer.getRootObject();

  serializeArbitrary<T>{}(serializer, rootObject, data);

  return std::move(serializer).write();
}

} // namespace blocks::serialization
