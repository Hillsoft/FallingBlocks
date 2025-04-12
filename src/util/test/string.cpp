#include <gtest/gtest.h>

#include <string_view>
#include "util/string.hpp"

TEST(StringTest, BasicTest) {
  EXPECT_EQ(util::toString("Hello, ", "world!"), "Hello, world!");
}

TEST(StringTest, BasicTest2) {
  const char* str1 = "Basic";
  EXPECT_EQ(util::toString(str1, " test"), "Basic test");
}

TEST(StringTest, StringViewTest) {
  std::string_view a = "Hello";
  std::string_view b = "World!";
  EXPECT_EQ(util::toString(a, " ", b), "Hello World!");
}
