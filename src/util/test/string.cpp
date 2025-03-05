#include <gtest/gtest.h>

#include "util/string.hpp"

TEST(StringTest, BasicTest) {
  EXPECT_EQ(util::toString("Hello, ", "world!"), "Hello, world!");
}

TEST(StringTest, BasicTest2) {
  const char* str1 = "Basic";
  EXPECT_EQ(util::toString(str1, " test"), "Basic test");
}
