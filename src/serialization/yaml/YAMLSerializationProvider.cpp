#include "serialization/yaml/YAMLSerializationProvider.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include "serialization/yaml/YAMLParser.hpp"
#include "serialization/yaml/YAMLTokenizer.hpp"
#include "util/debug.hpp"

namespace blocks::serialization::yaml {

YAMLDeserializationCursor::YAMLDeserializationCursor(YAMLDocument* document)
    : document_(document) {}

size_t YAMLDeserializationCursor::getStructFieldCount() {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  auto& mapping = std::get<YAMLDocument::Mapping>(document_->value_);

  return mapping.entries.size();
}

std::optional<YAMLDeserializationCursor>
YAMLDeserializationCursor::getSubFieldCursor(std::string_view name) {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  auto& mapping = std::get<YAMLDocument::Mapping>(document_->value_);

  for (auto& [fieldName, fieldValue] : mapping.entries) {
    if (fieldName == name) {
      return YAMLDeserializationCursor{&fieldValue};
    }
  }

  return std::nullopt;
}

YAMLDeserializationCursor::FieldIterator
YAMLDeserializationCursor::getFieldStartIterator() {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  return YAMLDeserializationCursor::FieldIterator{.target = this, .i = 0};
}

YAMLDeserializationCursor::FieldIterator
YAMLDeserializationCursor::getFieldEndIterator() {
  if (!std::holds_alternative<YAMLDocument::Mapping>(document_->value_)) {
    throw std::runtime_error{"Cannot get subfield of a non-map element"};
  }

  auto& mapping = std::get<YAMLDocument::Mapping>(document_->value_);

  return YAMLDeserializationCursor::FieldIterator{
      .target = this, .i = mapping.entries.size()};
}

size_t YAMLDeserializationCursor::getSequenceEntryCount() {
  if (!std::holds_alternative<YAMLDocument::Sequence>(document_->value_)) {
    throw std::runtime_error{
        "Cannot get sequence items of a non-sequence element"};
  }

  auto& sequence = std::get<YAMLDocument::Sequence>(document_->value_);

  return sequence.entries.size();
}

YAMLDeserializationCursor::SequenceIterator
YAMLDeserializationCursor::getSequenceStartIterator() {
  if (!std::holds_alternative<YAMLDocument::Sequence>(document_->value_)) {
    throw std::runtime_error{
        "Cannot get sequence items of a non-sequence element"};
  }

  return YAMLDeserializationCursor::SequenceIterator{.target = this, .i = 0};
}

YAMLDeserializationCursor::SequenceIterator
YAMLDeserializationCursor::getSequenceEndIterator() {
  if (!std::holds_alternative<YAMLDocument::Sequence>(document_->value_)) {
    throw std::runtime_error{
        "Cannot get sequence items of a non-sequence element"};
  }

  auto& sequence = std::get<YAMLDocument::Sequence>(document_->value_);

  return YAMLDeserializationCursor::SequenceIterator{
      .target = this, .i = sequence.entries.size()};
}

std::string_view YAMLDeserializationCursor::getStringValue() {
  if (!std::holds_alternative<YAMLDocument::LeafValue>(document_->value_)) {
    throw std::runtime_error{"Cannot get string value of non-leaf element"};
  }

  auto& leaf = std::get<YAMLDocument::LeafValue>(document_->value_);

  return leaf.value;
}

YAMLDeserializationProvider::YAMLDeserializationProvider(
    std::string_view dataStream)
    : document_(parseDocument(tokenizeYAML(dataStream))) {}

YAMLDeserializationProvider::TCursor
YAMLDeserializationProvider::getRootCursor() {
  return YAMLDeserializationCursor{&document_};
}

// NOLINTNEXTLINE(readability-make-member-function-const)
std::string_view YAMLDeserializationCursor::FieldIterator::fieldName() {
  return std::get<YAMLDocument::Mapping>(target->document_->value_)
      .entries[i]
      .first;
}

// NOLINTNEXTLINE(readability-make-member-function-const)
YAMLDeserializationCursor
YAMLDeserializationCursor::FieldIterator::fieldCursor() {
  return YAMLDeserializationCursor{&(
      std::get<YAMLDocument::Mapping>(target->document_->value_)
          .entries[i]
          .second)};
}

YAMLDeserializationCursor
// NOLINTNEXTLINE(readability-make-member-function-const)
YAMLDeserializationCursor::SequenceIterator::fieldCursor() {
  return YAMLDeserializationCursor{&(
      std::get<YAMLDocument::Sequence>(target->document_->value_).entries[i])};
}

void YAMLSerializationObject::setLeafValue(std::string value) {
  document_ = YAMLDocument{YAMLDocument::LeafValue{std::move(value)}};
}

void YAMLSerializationObject::pushSequenceElement(
    YAMLSerializationObject value) {
  DEBUG_ASSERT(value.document_.has_value());

  if (!document_.has_value()) {
    document_.emplace(YAMLDocument::Sequence{});
  }

  DEBUG_ASSERT(
      std::holds_alternative<YAMLDocument::Sequence>(document_->value_));

  auto& seq = std::get<YAMLDocument::Sequence>(document_->value_);
  seq.entries.emplace_back(*std::move(value).document_);
}

void YAMLSerializationObject::pushMappingEntry(
    std::string key, YAMLSerializationObject value) {
  DEBUG_ASSERT(value.document_.has_value());

  if (!document_.has_value()) {
    document_.emplace(YAMLDocument::Mapping{});
  }

  DEBUG_ASSERT(
      std::holds_alternative<YAMLDocument::Mapping>(document_->value_));

  auto& map = std::get<YAMLDocument::Mapping>(document_->value_);
  map.entries.emplace_back(std::move(key), *std::move(value).document_);
}

YAMLSerializationObject& YAMLSerializationProvider::getRootObject() {
  return rootObject_;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
YAMLSerializationObject YAMLSerializationProvider::makeSubObject() {
  return {};
}

std::string YAMLSerializationProvider::write() && {
  if (!rootObject_.document_.has_value()) {
    return "";
  }
  return writeDocument(*rootObject_.document_);
}

} // namespace blocks::serialization::yaml
