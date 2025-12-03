#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "serialization/yaml/YAMLParser.hpp"

namespace blocks::serialization::yaml {

class YAMLDeserializationCursor {
 public:
  struct FieldIterator {
    YAMLDeserializationCursor* target;
    size_t i;

    auto operator<=>(const FieldIterator& other) const = default;

    FieldIterator& operator++() {
      i++;
      return *this;
    }

    std::string_view fieldName();
    YAMLDeserializationCursor fieldCursor();
  };

  struct SequenceIterator {
    YAMLDeserializationCursor* target;
    size_t i;

    auto operator<=>(const SequenceIterator& other) const = default;

    SequenceIterator& operator++() {
      i++;
      return *this;
    }

    YAMLDeserializationCursor fieldCursor();
  };

  explicit YAMLDeserializationCursor(YAMLDocument* document);

  size_t getStructFieldCount();
  std::optional<YAMLDeserializationCursor> getSubFieldCursor(
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

class YAMLDeserializationProvider {
 public:
  using TCursor = YAMLDeserializationCursor;

  explicit YAMLDeserializationProvider(std::string_view dataStream);

  TCursor getRootCursor();

 private:
  YAMLDocument document_;
};

class YAMLSerializationObject {
 public:
  YAMLSerializationObject() = default;

  void setLeafValue(std::string value);
  void pushSequenceElement(YAMLSerializationObject element);
  void pushMappingEntry(std::string key, YAMLSerializationObject value);

  friend class YAMLSerializationProvider;

 private:
  std::optional<YAMLDocument> document_;
};

class YAMLSerializationProvider {
 public:
  using TObject = YAMLSerializationObject;

  YAMLSerializationProvider() = default;

  YAMLSerializationObject& getRootObject();
  YAMLSerializationObject makeSubObject();

  std::string write() &&;

 private:
  YAMLSerializationObject rootObject_;
};

} // namespace blocks::serialization::yaml
