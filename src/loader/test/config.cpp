#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "loader/Config.hpp"

TEST(ConfigTest, SimpleConfig) {
  std::string dataStr =
      "numKey=1234\nstrKey=Hello, world!\nlistKey=asdf;dfhd\n\n# Comment after an empty line";
  std::vector<char> dataVec{dataStr.begin(), dataStr.end()};

  blocks::loader::Config config{std::move(dataVec)};

  EXPECT_EQ(config.readString("strKey"), "Hello, world!");
  EXPECT_THROW(config.readString("otherKey"), std::runtime_error);
  EXPECT_EQ(config.readStringWithDefault("otherKey", "default"), "default");
  EXPECT_EQ(
      config.readStringList("listKey"),
      (std::vector<std::string_view>{"asdf", "dfhd"}));
}

TEST(ConfigTest, IntegerParsing) {
  std::string dataStr =
      "num1=1234\nneg=-58\nempty=\nnotint=asdf\nlist=23;987;0;-8";
  std::vector<char> dataVec{dataStr.begin(), dataStr.end()};

  blocks::loader::Config config{std::move(dataVec)};

  EXPECT_EQ(config.readInt("num1"), 1234);
  EXPECT_EQ(config.readInt("neg"), -58);
  EXPECT_THROW(config.readInt("empty"), std::runtime_error);
  EXPECT_THROW(config.readInt("notint"), std::runtime_error);
  EXPECT_EQ(config.readIntList("list"), (std::vector<long>{23, 987, 0, -8}));
}
