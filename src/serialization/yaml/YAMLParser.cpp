#include "serialization/yaml/YAMLParser.hpp"

#include <algorithm>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "serialization/yaml/YAMLTokenizer.hpp"
#include "util/debug.hpp"

namespace blocks::serialization::yaml {

namespace {

int effectiveCurrentIndentation(std::span<const YAMLSymbol> symbols) {
  if (symbols.empty()) {
    return 0;
  }

  if (!std::holds_alternative<YAMLSymbol::WhiteSpace>(symbols[0].value_)) {
    return std::holds_alternative<YAMLSymbol::BlockSequenceIndicator>(
               symbols[0].value_)
        ? 1
        : 0;
  }

  const auto& whiteSpaceDetail =
      std::get<YAMLSymbol::WhiteSpace>(symbols[0].value_);

  if (symbols.size() == 1 ||
      !std::holds_alternative<YAMLSymbol::BlockSequenceIndicator>(
          symbols[1].value_)) {
    return whiteSpaceDetail.indentSize;
  }

  return whiteSpaceDetail.indentSize + 1;
}

void consumeWhitespace(std::span<const YAMLSymbol>& symbols) {
  while (!symbols.empty() &&
         (std::holds_alternative<YAMLSymbol::WhiteSpace>(symbols[0].value_) ||
          std::holds_alternative<YAMLSymbol::Comment>(symbols[0].value_))) {
    symbols = symbols.subspan(1);
  }
}

YAMLDocument parseObject(
    int indentLimit, bool isMidLine, std::span<const YAMLSymbol>& symbols);

YAMLDocument emptyDocument() {
  return YAMLDocument{YAMLDocument::LeafValue{std::string{""}}};
}

YAMLDocument parseLeafValue(
    int indentLimit, std::span<const YAMLSymbol>& symbols) {
  consumeWhitespace(symbols);
  if (symbols.empty()) {
    return YAMLDocument{YAMLDocument::LeafValue{std::string{""}}};
  }

  if (std::holds_alternative<YAMLSymbol::ScalarText>(symbols[0].value_)) {
    const auto& textData = std::get<YAMLSymbol::ScalarText>(symbols[0].value_);
    symbols = symbols.subspan(1);
    return YAMLDocument{
        YAMLDocument::LeafValue{std::string{textData.contents}}};
  }

  if (!std::holds_alternative<YAMLSymbol::NewLine>(symbols[0].value_)) {
    throw std::runtime_error{"Expected leaf value"};
  }

  if (effectiveCurrentIndentation(symbols.subspan(1)) < indentLimit) {
    return YAMLDocument{YAMLDocument::LeafValue{std::string{""}}};
  }

  symbols = symbols.subspan(1);
  consumeWhitespace(symbols);

  if (std::holds_alternative<YAMLSymbol::ScalarText>(symbols[0].value_)) {
    const auto& textData = std::get<YAMLSymbol::ScalarText>(symbols[0].value_);
    symbols = symbols.subspan(1);
    return YAMLDocument{
        YAMLDocument::LeafValue{std::string{textData.contents}}};
  }

  throw std::runtime_error{"Expected leaf value"};
}

YAMLDocument parseSequence(
    int indentLimit, std::span<const YAMLSymbol>& symbols) {
  std::vector<YAMLDocument> elements;

  consumeWhitespace(symbols);

  while (true) {
    if (symbols.empty()) {
      return YAMLDocument{YAMLDocument::Sequence{std::move(elements)}};
    }

    if (!std::holds_alternative<YAMLSymbol::BlockSequenceIndicator>(
            symbols[0].value_)) {
      throw std::runtime_error{"Expected sequence"};
    }
    symbols = symbols.subspan(1);
    consumeWhitespace(symbols);

    elements.emplace_back(parseObject(indentLimit + 1, true, symbols));

    consumeWhitespace(symbols);

    if (symbols.empty()) {
      return YAMLDocument{YAMLDocument::Sequence{std::move(elements)}};
    }

    if (!std::holds_alternative<YAMLSymbol::NewLine>(symbols[0].value_)) {
      throw std::runtime_error{"Improperly formed sequence"};
    }

    const int nextIndent = effectiveCurrentIndentation(symbols.subspan(1));
    if (nextIndent > indentLimit) {
      throw std::runtime_error{"Improperly formed sequence"};
    } else if (nextIndent < indentLimit) {
      return YAMLDocument{YAMLDocument::Sequence{std::move(elements)}};
    }

    symbols = symbols.subspan(1);
    consumeWhitespace(symbols);
  }
}

YAMLDocument parseMapping(
    int indentLimit, std::span<const YAMLSymbol>& symbols) {
  std::vector<std::pair<std::string, YAMLDocument>> elements;

  consumeWhitespace(symbols);

  while (true) {
    if (symbols.empty()) {
      return YAMLDocument{YAMLDocument::Mapping{std::move(elements)}};
    }

    if (!std::holds_alternative<YAMLSymbol::ScalarText>(symbols[0].value_)) {
      throw std::runtime_error{"Expected mapping key"};
    }
    std::string key{
        std::get<YAMLSymbol::ScalarText>(symbols[0].value_).contents};

    symbols = symbols.subspan(1);
    consumeWhitespace(symbols);

    if (symbols.empty() ||
        !std::holds_alternative<YAMLSymbol::MappingValueSeparator>(
            symbols[0].value_)) {
      throw std::runtime_error{"Expected mapping value"};
    }

    symbols = symbols.subspan(1);
    consumeWhitespace(symbols);

    elements.emplace_back(
        std::move(key), parseObject(indentLimit + 1, true, symbols));

    consumeWhitespace(symbols);

    if (symbols.empty()) {
      return YAMLDocument{YAMLDocument::Mapping{std::move(elements)}};
    }

    if (!std::holds_alternative<YAMLSymbol::NewLine>(symbols[0].value_)) {
      throw std::runtime_error{"Improperly formed mapping"};
    }

    const int nextIndent = effectiveCurrentIndentation(symbols.subspan(1));
    if (nextIndent > indentLimit) {
      throw std::runtime_error{"Improperly formed mapping"};
    } else if (nextIndent < indentLimit) {
      return YAMLDocument{YAMLDocument::Mapping{std::move(elements)}};
    }

    symbols = symbols.subspan(1);
    consumeWhitespace(symbols);
  }
}

YAMLDocument parseObject(
    int indentLimit, bool isMidLine, std::span<const YAMLSymbol>& symbols) {
  if (isMidLine) {
    consumeWhitespace(symbols);
    if (symbols.empty() ||
        !std::holds_alternative<YAMLSymbol::NewLine>(symbols[0].value_)) {
      return parseLeafValue(indentLimit, symbols);
    }

    symbols = symbols.subspan(1);
  }

  const int currentIndent = effectiveCurrentIndentation(symbols);
  if (currentIndent < 0) {
    throw std::runtime_error{"Improper indentation; only spaces are valid"};
  }

  if (currentIndent < indentLimit) {
    return emptyDocument();
  }

  indentLimit = currentIndent;

  consumeWhitespace(symbols);
  if (symbols.empty()) {
    return emptyDocument();
  }

  if (std::holds_alternative<YAMLSymbol::BlockSequenceIndicator>(
          symbols[0].value_)) {
    return parseSequence(indentLimit, symbols);
  }

  std::span<const YAMLSymbol> advance = symbols.subspan(1);
  consumeWhitespace(advance);
  if (!advance.empty() &&
      std::holds_alternative<YAMLSymbol::MappingValueSeparator>(
          advance[0].value_)) {
    return parseMapping(indentLimit, symbols);
  }

  return parseLeafValue(indentLimit, symbols);
}

std::string paddingString(int length) {
  std::string result;
  result.resize(length, ' ');
  return result;
}

void writeDocumentImpl(
    std::string& out, int indent, const YAMLDocument& document) {
  if (std::holds_alternative<YAMLDocument::Mapping>(document.value_)) {
    const std::string indentString = paddingString(indent);
    const auto& value = std::get<YAMLDocument::Mapping>(document.value_);
    for (const auto& [key, mapElem] : value.entries) {
      out.append(indentString);
      out.append(key);
      out.append(":\n");
      writeDocumentImpl(out, indent + 2, mapElem);
    }
  } else if (std::holds_alternative<YAMLDocument::Sequence>(document.value_)) {
    const std::string indentString =
        paddingString(std::max(0, indent - 2)) + "- \n";
    const auto& value = std::get<YAMLDocument::Sequence>(document.value_);
    for (const auto& seqElem : value.entries) {
      out.append(indentString);
      writeDocumentImpl(out, indent + 2, seqElem);
    }
  } else if (std::holds_alternative<YAMLDocument::LeafValue>(document.value_)) {
    out.append(paddingString(indent));
    out.append(std::get<YAMLDocument::LeafValue>(document.value_).value);
    out.append("\n");
  } else {
    DEBUG_ASSERT(false);
  }
}

} // namespace

YAMLDocument parseDocument(std::span<const YAMLSymbol> symbols) {
  YAMLDocument result = parseObject(0, false, symbols);

  for (const auto& s : symbols) {
    if (!std::holds_alternative<YAMLSymbol::WhiteSpace>(s.value_) &&
        !std::holds_alternative<YAMLSymbol::Comment>(s.value_) &&
        !std::holds_alternative<YAMLSymbol::NewLine>(s.value_)) {
      throw std::runtime_error{"Expected end of file"};
    }
  }

  return result;
}

std::string writeDocument(const YAMLDocument& document) {
  std::string result;
  writeDocumentImpl(result, 0, document);
  return result;
}

bool YAMLDocument::LeafValue::operator==(
    const YAMLDocument::LeafValue& other) const = default;
bool YAMLDocument::Sequence::operator==(
    const YAMLDocument::Sequence& other) const = default;
bool YAMLDocument::Mapping::operator==(
    const YAMLDocument::Mapping& other) const = default;

} // namespace blocks::serialization::yaml
