#include "serialization/yaml/YAMLSerializationProvider.hpp"

#include <optional>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>
#include "serialization/yaml/YAMLParser.hpp"
#include "serialization/yaml/YAMLTokenizer.hpp"

namespace blocks::serialization::yaml {

YAMLSerializationCursor::YAMLSerializationCursor(YAMLDocument* document)
    : document_(document) {}

size_t YAMLSerializationCursor::getStructFieldCount() {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  YAMLDocument::Mapping& mapping =
      std::get<YAMLDocument::Mapping>(document_->value_);

  return mapping.entries.size();
}

std::optional<YAMLSerializationCursor>
YAMLSerializationCursor::getSubFieldCursor(std::string_view name) {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  YAMLDocument::Mapping& mapping =
      std::get<YAMLDocument::Mapping>(document_->value_);

  for (auto& [fieldName, fieldValue] : mapping.entries) {
    if (fieldName == name) {
      return YAMLSerializationCursor{&fieldValue};
    }
  }

  return std::nullopt;
}

YAMLSerializationCursor::FieldIterator
YAMLSerializationCursor::getFieldStartIterator() {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  return YAMLSerializationCursor::FieldIterator{this, 0};
}

YAMLSerializationCursor::FieldIterator
YAMLSerializationCursor::getFieldEndIterator() {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  YAMLDocument::Mapping& mapping =
      std::get<YAMLDocument::Mapping>(document_->value_);

  return YAMLSerializationCursor::FieldIterator{this, mapping.entries.size()};
}

size_t YAMLSerializationCursor::getSequenceEntryCount() {
  if (!std::holds_alternative<YAMLDocument::Sequence>(document_->value_)) {
    throw std::runtime_error{
        "Cannot get sequence items of a non-sequence element"};
  }

  YAMLDocument::Sequence& sequence =
      std::get<YAMLDocument::Sequence>(document_->value_);

  return sequence.entries.size();
}

YAMLSerializationCursor::SequenceIterator
YAMLSerializationCursor::getSequenceStartIterator() {
  if (!std::holds_alternative<YAMLDocument::Sequence>(document_->value_)) {
    throw std::runtime_error{
        "Cannot get sequence items of a non-sequence element"};
  }

  return YAMLSerializationCursor::SequenceIterator{this, 0};
}

YAMLSerializationCursor::SequenceIterator
YAMLSerializationCursor::getSequenceEndIterator() {
  if (!std::holds_alternative<YAMLDocument::Sequence>(document_->value_)) {
    throw std::runtime_error{
        "Cannot get sequence items of a non-sequence element"};
  }

  YAMLDocument::Sequence& sequence =
      std::get<YAMLDocument::Sequence>(document_->value_);

  return YAMLSerializationCursor::SequenceIterator{
      this, sequence.entries.size()};
}

std::string_view YAMLSerializationCursor::getStringValue() {
  if (!std::holds_alternative<YAMLDocument::LeafValue>(document_->value_)) {
    throw std::runtime_error{"Cannot get string value of non-leaf element"};
  }

  YAMLDocument::LeafValue& leaf =
      std::get<YAMLDocument::LeafValue>(document_->value_);

  return leaf.value;
}

YAMLSerializationProvider::YAMLSerializationProvider(
    std::string_view dataStream)
    : document_(parseDocument(tokenizeYAML(dataStream))) {}

YAMLSerializationProvider::TCursor YAMLSerializationProvider::getRootCursor() {
  return YAMLSerializationCursor{&document_};
}

std::string_view YAMLSerializationCursor::FieldIterator::fieldName() {
  return std::get<YAMLDocument::Mapping>(target->document_->value_)
      .entries[i]
      .first;
}

YAMLSerializationCursor YAMLSerializationCursor::FieldIterator::fieldCursor() {
  return YAMLSerializationCursor{&(
      std::get<YAMLDocument::Mapping>(target->document_->value_)
          .entries[i]
          .second)};
}

YAMLSerializationCursor
YAMLSerializationCursor::SequenceIterator::fieldCursor() {
  return YAMLSerializationCursor{&(
      std::get<YAMLDocument::Sequence>(target->document_->value_).entries[i])};
}

} // namespace blocks::serialization::yaml
