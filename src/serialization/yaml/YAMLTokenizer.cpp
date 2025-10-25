#include "serialization/yaml/YAMLTokenizer.hpp"

#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>
#include "util/array.hpp"
#include "util/string.hpp"

namespace blocks::serialization::yaml {

namespace {

struct ParseResult {
  size_t bytesParsed;
  YAMLSymbol symbol;
};

bool isWhiteSpace(char c) {
  constexpr auto whiteSpaceChars = util::makeArray<char>(' ', '\t');
  for (const auto& ws : whiteSpaceChars) {
    if (ws == c) {
      return true;
    }
  }
  return false;
}

bool isPrintable(char c) {
  return c >= 32 && c <= 126;
}

std::optional<ParseResult> parseNewLine(std::string_view yamlSource) {
  constexpr char lineBreak = '\n';
  constexpr char carriageReturn = '\r';

  if (yamlSource.size() >= 2 && yamlSource[0] == carriageReturn &&
      yamlSource[1] == lineBreak) {
    return ParseResult{2, YAMLSymbol::NewLine{}};
  }

  if (yamlSource[0] == lineBreak || yamlSource[0] == carriageReturn) {
    return ParseResult{1, YAMLSymbol::NewLine{}};
  }

  return std::nullopt;
}

std::optional<ParseResult> parseWhiteSpace(std::string_view yamlSource) {
  int indentSize = 0;

  size_t i = 0;
  for (; i < yamlSource.size() && isWhiteSpace(yamlSource[i]); i++) {
    if (indentSize != -1) {
      if (yamlSource[i] == ' ') {
        indentSize++;
      } else {
        indentSize = -1;
      }
    }
  }

  if (i > 0) {
    return ParseResult{i, YAMLSymbol::WhiteSpace{indentSize}};
  }

  return std::nullopt;
}

std::optional<ParseResult> parseBlockSequenceIndicator(
    std::string_view yamlSource) {
  if (yamlSource[0] == '-') {
    return ParseResult{1, YAMLSymbol::BlockSequenceIndicator{}};
  }

  return std::nullopt;
}

std::optional<ParseResult> parseMappingKeyIndicator(
    std::string_view yamlSource) {
  if (yamlSource[0] == '?') {
    return ParseResult{1, YAMLSymbol::MappingKeyIndicator{}};
  }

  return std::nullopt;
}

std::optional<ParseResult> parseMappingValueSeparator(
    std::string_view yamlSource) {
  if (yamlSource[0] == ':') {
    return ParseResult{1, YAMLSymbol::MappingValueSeparator{}};
  }

  return std::nullopt;
}

std::optional<ParseResult> parsePlainScalar(std::string_view yamlSource) {
  // is first character unsafe
  if (!isPrintable(yamlSource[0]) || isWhiteSpace(yamlSource[0]) ||
      yamlSource[0] == ':' || yamlSource[0] == '?' || yamlSource[0] == '-' ||
      yamlSource[0] == '#') {
    return std::nullopt;
  }

  size_t i = 1;
  for (; i < yamlSource.size(); i++) {
    if (yamlSource[i] == '\n' || yamlSource[i] == '\r') {
      break;
    }
    if (yamlSource[i] == '#' && isWhiteSpace(yamlSource[i - 1])) {
      break;
    }
    if (yamlSource[i] == ':' &&
        (yamlSource.size() > i + 1 || isWhiteSpace(yamlSource[i + 1]))) {
      break;
    }
  }

  // Remove trailing whitespace
  for (; isWhiteSpace(yamlSource[i - 1]); i--) {
  }

  return ParseResult{i, YAMLSymbol::ScalarText{yamlSource.substr(0, i)}};
}

std::optional<ParseResult> parseComment(std::string_view yamlSource) {
  if (yamlSource[0] != '#') {
    return std::nullopt;
  }

  size_t i = 1;
  for (; i < yamlSource.size(); i++) {
    if (yamlSource[i] == '\n' || yamlSource[i] == '\r') {
      break;
    }
  }

  return ParseResult{i, YAMLSymbol::Comment{}};
}

using Parser = std::optional<ParseResult> (*)(std::string_view);

constexpr auto parserList = util::makeArray<Parser>(
    parseNewLine,
    parseWhiteSpace,
    parseBlockSequenceIndicator,
    parseMappingKeyIndicator,
    parseMappingValueSeparator,
    parsePlainScalar,
    parseComment);

} // namespace

std::vector<YAMLSymbol> tokenizeYAML(std::string_view yamlSource) {
  size_t pos = 0;
  std::vector<YAMLSymbol> result;

  while (yamlSource.size() > 0) {
    std::optional<ParseResult> nextSymbol;

    for (const auto& p : parserList) {
      std::optional<ParseResult> curResult = p(yamlSource);
      if (curResult.has_value()) {
        if (!nextSymbol.has_value() ||
            curResult->bytesParsed > nextSymbol->bytesParsed) {
          nextSymbol = std::move(curResult);
        }
      }
    }

    if (!nextSymbol.has_value()) {
      throw std::runtime_error{util::toString("YAML parse error at pos ", pos)};
    }

    pos += nextSymbol->bytesParsed;
    yamlSource = yamlSource.substr(nextSymbol->bytesParsed);
    result.emplace_back(std::move(nextSymbol->symbol));
  }

  return result;
}

} // namespace blocks::serialization::yaml
