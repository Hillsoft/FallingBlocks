#include <gtest/gtest.h>

#include <string_view>
#include <vector>
#include "serialization/yaml/YAMLTokenizer.hpp"

template <typename... TArgs>
std::vector<blocks::serialization::yaml::YAMLSymbol> makeSymbolSequence(
    TArgs... args) {
  std::vector<blocks::serialization::yaml::YAMLSymbol> result;

  auto addArg = [&](auto&& arg) {
    result.emplace_back(blocks::serialization::yaml::YAMLSymbol{arg});
  };

  (addArg(args), ...);
  return result;
}

TEST(YAMLTokenizerTest, Sequence) {
  std::string_view inputYAML =
      "- Mark McGwire\n"
      "- Sammy Sosa\n"
      "- Ken Griffey";

  std::vector<blocks::serialization::yaml::YAMLSymbol> result =
      blocks::serialization::yaml::tokenizeYAML(inputYAML);
  EXPECT_EQ(
      makeSymbolSequence(
          blocks::serialization::yaml::YAMLSymbol::BlockSequenceIndicator{},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"Mark McGwire"},
          blocks::serialization::yaml::YAMLSymbol::NewLine{},
          blocks::serialization::yaml::YAMLSymbol::BlockSequenceIndicator{},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"Sammy Sosa"},
          blocks::serialization::yaml::YAMLSymbol::NewLine{},
          blocks::serialization::yaml::YAMLSymbol::BlockSequenceIndicator{},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"Ken Griffey"}),
      result);
}

TEST(YAMLTokenizerTest, Mapping) {
  std::string_view inputYAML =
      "hr:  65    # Home runs\n"
      "avg: 0.278 # Batting average\n"
      "rbi: 147   # Runs batted in";

  std::vector<blocks::serialization::yaml::YAMLSymbol> result =
      blocks::serialization::yaml::tokenizeYAML(inputYAML);
  EXPECT_EQ(
      makeSymbolSequence(
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"hr"},
          blocks::serialization::yaml::YAMLSymbol::MappingValueSeparator{},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"65"},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::Comment{},
          blocks::serialization::yaml::YAMLSymbol::NewLine{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"avg"},
          blocks::serialization::yaml::YAMLSymbol::MappingValueSeparator{},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"0.278"},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::Comment{},
          blocks::serialization::yaml::YAMLSymbol::NewLine{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"rbi"},
          blocks::serialization::yaml::YAMLSymbol::MappingValueSeparator{},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"147"},
          blocks::serialization::yaml::YAMLSymbol::WhiteSpace{},
          blocks::serialization::yaml::YAMLSymbol::Comment{}),
      result);
}
