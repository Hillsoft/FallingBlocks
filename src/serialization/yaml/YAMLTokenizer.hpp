#pragma once

#include <string_view>
#include <variant>
#include <vector>

namespace blocks::serialization::yaml {

struct YAMLSymbol {
  struct NewLine {
    bool operator==(const NewLine& other) const = default;
  };
  struct WhiteSpace {
    int indentSize;
    bool operator==(const WhiteSpace& other) const = default;
  };
  struct BlockSequenceIndicator {
    bool operator==(const BlockSequenceIndicator& other) const = default;
  };
  struct MappingKeyIndicator {
    bool operator==(const MappingKeyIndicator& other) const = default;
  };
  struct MappingValueSeparator {
    bool operator==(const MappingValueSeparator& other) const = default;
  };
  struct ScalarText {
    std::string_view contents;
    bool operator==(const ScalarText& other) const = default;
  };
  struct Comment {
    bool operator==(const Comment& other) const = default;
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

  bool operator==(const YAMLSymbol& other) const = default;
};

std::vector<YAMLSymbol> tokenizeYAML(std::string_view yamlSource);

} // namespace blocks::serialization::yaml
