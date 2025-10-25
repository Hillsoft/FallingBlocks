#pragma once

#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "serialization/yaml/YAMLTokenizer.hpp"

namespace blocks::serialization::yaml {

struct YAMLDocument {
  struct LeafValue {
    auto operator<=>(const LeafValue& other) const = default;
    std::string value;
  };
  struct Sequence {
    auto operator<=>(const Sequence& other) const = default;
    std::vector<YAMLDocument> entries;
  };
  struct Mapping {
    auto operator<=>(const Mapping& other) const = default;
    std::vector<std::pair<std::string, YAMLDocument>> entries;
  };

  auto operator<=>(const YAMLDocument& other) const = default;

  std::variant<LeafValue, Sequence, Mapping> value_;
};

YAMLDocument parseDocument(std::span<const YAMLSymbol> symbols);

} // namespace blocks::serialization::yaml
