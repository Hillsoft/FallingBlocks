#pragma once

#include <optional>
#include <string_view>
#include <vector>
#include "serialization/yaml/YAMLParser.hpp"

namespace blocks::serialization::yaml {

class YAMLSerializationCursor {
 public:
  struct FieldIterator {
    YAMLSerializationCursor* target;
    size_t i;

    auto operator<=>(const FieldIterator& other) const = default;

    FieldIterator& operator++() {
      i++;
      return *this;
    }

    std::string_view fieldName();
    YAMLSerializationCursor fieldCursor();
  };

  struct SequenceIterator {
    YAMLSerializationCursor* target;
    size_t i;

    auto operator<=>(const SequenceIterator& other) const = default;

    SequenceIterator& operator++() {
      i++;
      return *this;
    }

    YAMLSerializationCursor fieldCursor();
  };

  YAMLSerializationCursor(YAMLDocument* document);

  size_t getStructFieldCount();
  std::optional<YAMLSerializationCursor> getSubFieldCursor(
      std::string_view name);
  FieldIterator getFieldStartIterator();
  FieldIterator getFieldEndIterator();

  size_t getSequenceEntryCount();
  SequenceIterator getSequenceStartIterator();
  SequenceIterator getSequenceEndIterator();

  std::string_view getStringValue();

 private:
  YAMLDocument* document_;
};

class YAMLSerializationProvider {
 public:
  using TCursor = YAMLSerializationCursor;

  YAMLSerializationProvider(std::string_view dataStream);

  TCursor getRootCursor();

 private:
  YAMLDocument document_;
};

} // namespace blocks::serialization::yaml
