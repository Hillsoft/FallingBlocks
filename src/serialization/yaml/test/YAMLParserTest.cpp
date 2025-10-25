#include <gtest/gtest.h>

#include <string_view>
#include <vector>
#include "serialization/yaml/YAMLParser.hpp"
#include "serialization/yaml/YAMLTokenizer.hpp"

TEST(YAMLTokenizerTest, Sequence) {
  std::string_view inputYAML =
      "- Mark McGwire\n"
      "- Sammy Sosa\n"
      "- Ken Griffey";

  std::vector<blocks::serialization::yaml::YAMLSymbol> tokens =
      blocks::serialization::yaml::tokenizeYAML(inputYAML);
  blocks::serialization::yaml::YAMLDocument result =
      blocks::serialization::yaml::parseDocument(tokens);

  blocks::serialization::yaml::YAMLDocument expected{
      blocks::serialization::yaml::YAMLDocument::Sequence{std::vector<
          blocks::serialization::yaml::YAMLDocument>{
          blocks::serialization::yaml::YAMLDocument{
              blocks::serialization::yaml::YAMLDocument::LeafValue{
                  "Mark McGwire"}},
          blocks::serialization::yaml::YAMLDocument{
              blocks::serialization::yaml::YAMLDocument::LeafValue{
                  "Sammy Sosa"}},
          blocks::serialization::yaml::YAMLDocument{
              blocks::serialization::yaml::YAMLDocument::LeafValue{
                  "Ken Griffey"}}}}};
  EXPECT_EQ(expected, result);
}

TEST(YAMLTokenizerTest, Mapping) {
  std::string_view inputYAML =
      "hr:  65    # Home runs\n"
      "avg: 0.278 # Batting average\n"
      "rbi: 147   # Runs batted in";

  std::vector<blocks::serialization::yaml::YAMLSymbol> tokens =
      blocks::serialization::yaml::tokenizeYAML(inputYAML);
  blocks::serialization::yaml::YAMLDocument result =
      blocks::serialization::yaml::parseDocument(tokens);

  blocks::serialization::yaml::YAMLDocument expected{
      blocks::serialization::yaml::YAMLDocument::Mapping{
          {{"hr",
            blocks::serialization::yaml::YAMLDocument{
                blocks::serialization::yaml::YAMLDocument::LeafValue{"65"}}},
           {"avg",
            blocks::serialization::yaml::YAMLDocument{
                blocks::serialization::yaml::YAMLDocument::LeafValue{"0.278"}}},
           {"rbi",
            blocks::serialization::yaml::YAMLDocument{
                blocks::serialization::yaml::YAMLDocument::LeafValue{
                    "147"}}}}}};

  EXPECT_EQ(expected, result);
}

TEST(YAMLTokenizerTest, MappingToSequence) {
  std::string_view inputYAML =
      "american:\n"
      "- Boston Red Sox\n"
      "- Detroit Tigers\n"
      "- New York Yankees\n"
      "national:\n"
      "- New York Mets\n"
      "- Chicago Cubs\n"
      "- Atlanta Braves";

  std::vector<blocks::serialization::yaml::YAMLSymbol> tokens =
      blocks::serialization::yaml::tokenizeYAML(inputYAML);
  blocks::serialization::yaml::YAMLDocument result =
      blocks::serialization::yaml::parseDocument(tokens);

  blocks::serialization::yaml::YAMLDocument expected{
      blocks::serialization::yaml::YAMLDocument::Mapping{
          {{"american",
            blocks::serialization::yaml::YAMLDocument{
                blocks::serialization::yaml::YAMLDocument::Sequence{
                    {blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "Boston Red Sox"}},
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "Detroit Tigers"}},
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "New York Yankees"}}}}}},
           {"national",
            blocks::serialization::yaml::YAMLDocument{
                blocks::serialization::yaml::YAMLDocument::Sequence{
                    {blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "New York Mets"}},
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "Chicago Cubs"}},
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "Atlanta Braves"}}}}}}}}};

  EXPECT_EQ(expected, result);
}

TEST(YAMLTokenizerTest, SequenceOfMappings) {
  std::string_view inputYAML =
      "-\n"
      "  name: Mark McGwire\n"
      "  hr:   65\n"
      "  avg:  0.278\n"
      "-\n"
      "  name: Sammy Sosa\n"
      "  hr:   63\n"
      "  avg:  0.288";

  std::vector<blocks::serialization::yaml::YAMLSymbol> tokens =
      blocks::serialization::yaml::tokenizeYAML(inputYAML);
  blocks::serialization::yaml::YAMLDocument result =
      blocks::serialization::yaml::parseDocument(tokens);

  blocks::serialization::yaml::YAMLDocument expected{
      blocks::serialization::yaml::YAMLDocument::Sequence{
          {blocks::serialization::yaml::YAMLDocument{
               blocks::serialization::yaml::YAMLDocument::Mapping{
                   {{"name",
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "Mark McGwire"}}},
                    {"hr",
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "65"}}},
                    {"avg",
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "0.278"}}}}}},
           blocks::serialization::yaml::YAMLDocument{
               blocks::serialization::yaml::YAMLDocument::Mapping{
                   {{"name",
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "Sammy Sosa"}}},
                    {"hr",
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "63"}}},
                    {"avg",
                     blocks::serialization::yaml::YAMLDocument{
                         blocks::serialization::yaml::YAMLDocument::LeafValue{
                             "0.288"}}}}}}}}};

  EXPECT_EQ(expected, result);
}
