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
    bool operator==(const LeafValue& other) const;
    std::string value;
  };
  struct Sequence {
    bool operator==(const Sequence& other) const;
    std::vector<YAMLDocument> entries;
  };
  struct Mapping {
    bool operator==(const Mapping& other) const;
    std::vector<std::pair<std::string, YAMLDocument>> entries;
  };

  bool operator==(const YAMLDocument& other) const = default;

  std::variant<LeafValue, Sequence, Mapping> value_;
};

YAMLDocument parseDocument(std::span<const YAMLSymbol> symbols);

std::string writeDocument(const YAMLDocument& document);

} // namespace blocks::serialization::yaml
