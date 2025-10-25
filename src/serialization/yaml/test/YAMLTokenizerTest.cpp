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
          blocks::serialization::yaml::YAMLSymbol::ScalarText{"Ken Griffey"},
          blocks::serialization::yaml::YAMLSymbol::NewLine{}),
      result);
}
