#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <memory>
#include <span>
#include "util/Generator.hpp"
#include "util/vec_generators.hpp"

util::Generator<int> iterateInts() {
  for (int i = 0; true; i++) {
    co_yield i;
  }
}

TEST(GeneratorTest, iterateIntTest) {
  util::Generator<int> intGen = iterateInts();
  EXPECT_EQ(intGen(), 0);
  EXPECT_EQ(intGen(), 1);
  EXPECT_EQ(intGen(), 2);
}

util::Generator<int> iterateDoubles() {
  util::Generator<int> intGen = iterateInts();
  while (!intGen.isDone()) {
    co_yield 2 * intGen();
  }
}

TEST(GeneratorTest, iterateDoubles) {
  util::Generator<int> intGen = iterateDoubles();
  EXPECT_EQ(intGen(), 0);
  EXPECT_EQ(intGen(), 2);
  EXPECT_EQ(intGen(), 4);
}

util::Generator<int> sharedPtrPassthrough(std::shared_ptr<int> ptr) {
  co_return;
}

TEST(GeneratorTest, CleanupTest) {
  std::shared_ptr<int> ptr = std::make_shared<int>(0);
  EXPECT_EQ(ptr.use_count(), 1);

  {
    util::Generator<int> genHolder = sharedPtrPassthrough(ptr);
    EXPECT_EQ(ptr.use_count(), 2);
  }

  EXPECT_EQ(ptr.use_count(), 1);
}

util::Generator<int> upToThree() {
  co_yield 1;
  co_yield 2;
  co_yield 3;
}

TEST(GeneratorTest, UpToThree) {
  util::Generator<int> intGen = upToThree();
  EXPECT_EQ(intGen(), 1);
  EXPECT_EQ(intGen(), 2);
  EXPECT_EQ(intGen(), 3);
  EXPECT_TRUE(intGen.isDone());
}
