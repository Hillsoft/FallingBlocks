#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "serialization/Serialization.hpp"
#include "serialization/yaml/YAMLSerializationProvider.hpp"
#include "util/meta_utils.hpp"

struct ScoreDetails {
  bool operator==(const ScoreDetails& other) const = default;

  std::string homeRuns;
  std::string average;
  std::string runs;

  using Fields = util::TArray<
      util::TPair<util::TString<"hr">, std::string>,
      util::TPair<util::TString<"avg">, std::string>,
      util::TPair<util::TString<"rbi">, std::string>>;
};

struct PlayerDetails {
  bool operator==(const PlayerDetails& other) const = default;

  std::string name;
  std::string homeRuns;
  std::string average;

  using Fields = util::TArray<
      util::TPair<util::TString<"name">, std::string>,
      util::TPair<util::TString<"hr">, std::string>,
      util::TPair<util::TString<"avg">, std::string>>;
};

TEST(YAMLSerializationTest, Sequence) {
  std::string_view inputYAML =
      "- Mark McGwire\n"
      "- Sammy Sosa\n"
      "- Ken Griffey";

  std::vector<std::string> result = blocks::serialization::deserialize<
      std::vector<std::string>,
      blocks::serialization::yaml::YAMLSerializationProvider>(inputYAML);

  std::vector<std::string> expected{
      "Mark McGwire", "Sammy Sosa", "Ken Griffey"};
  EXPECT_EQ(expected, result);
}

TEST(YAMLSerializationTest, Mapping) {
  std::string_view inputYAML =
      "hr:  65    # Home runs\n"
      "avg: 0.278 # Batting average\n"
      "rbi: 147   # Runs batted in";

  ScoreDetails result = blocks::serialization::deserialize<
      ScoreDetails,
      blocks::serialization::yaml::YAMLSerializationProvider>(inputYAML);

  ScoreDetails expected{"65", "0.278", "147"};
  EXPECT_EQ(expected, result);
}

TEST(YAMLSerializationTest, MappingToSequence) {
  std::string_view inputYAML =
      "american:\n"
      "- Boston Red Sox\n"
      "- Detroit Tigers\n"
      "- New York Yankees\n"
      "national:\n"
      "- New York Mets\n"
      "- Chicago Cubs\n"
      "- Atlanta Braves";

  std::unordered_map<std::string, std::vector<std::string>> result =
      blocks::serialization::deserialize<
          std::unordered_map<std::string, std::vector<std::string>>,
          blocks::serialization::yaml::YAMLSerializationProvider>(inputYAML);

  std::unordered_map<std::string, std::vector<std::string>> expected{
      {"american", {"Boston Red Sox", "Detroit Tigers", "New York Yankees"}},
      {"national", {"New York Mets", "Chicago Cubs", "Atlanta Braves"}}};
  EXPECT_EQ(expected, result);
}

TEST(YAMLSerializationTest, SequenceToMapping) {
  std::string_view inputYAML =
      "-\n"
      "  name: Mark McGwire\n"
      "  hr:   65\n"
      "  avg:  0.278\n"
      "-\n"
      "  name: Sammy Sosa\n"
      "  hr:   63\n"
      "  avg:  0.288";

  std::vector<PlayerDetails> result = blocks::serialization::deserialize<
      std::vector<PlayerDetails>,
      blocks::serialization::yaml::YAMLSerializationProvider>(inputYAML);

  std::vector<PlayerDetails> expected{
      {"Mark McGwire", "65", "0.278"}, {"Sammy Sosa", "63", "0.288"}};
  EXPECT_EQ(expected, result);
}
