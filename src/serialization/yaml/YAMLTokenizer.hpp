#pragma once

#include <string_view>
#include <variant>
#include <vector>

namespace blocks::serialization::yaml {

struct YAMLSymbol {
  struct NewLine {
    auto operator<=>(const NewLine& other) const = default;
  };
  struct WhiteSpace {
    int indentSize;
    auto operator<=>(const WhiteSpace& other) const = default;
  };
  struct BlockSequenceIndicator {
    auto operator<=>(const BlockSequenceIndicator& other) const = default;
  };
  struct MappingKeyIndicator {
    auto operator<=>(const MappingKeyIndicator& other) const = default;
  };
  struct MappingValueSeparator {
    auto operator<=>(const MappingValueSeparator& other) const = default;
  };
  struct ScalarText {
    std::string_view contents;
    auto operator<=>(const ScalarText& other) const = default;
  };
  struct Comment {
    auto operator<=>(const Comment& other) const = default;
  };

  std::variant<
      NewLine,
      WhiteSpace,
      BlockSequenceIndicator,
      MappingKeyIndicator,
      MappingValueSeparator,
      ScalarText,
      Comment>
      value_;

  auto operator<=>(const YAMLSymbol& other) const = default;
};

std::vector<YAMLSymbol> tokenizeYAML(std::string_view yamlSource);

} // namespace blocks::serialization::yaml
