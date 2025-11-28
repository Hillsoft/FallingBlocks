#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <memory>
#include <span>
#include <stdexcept>
#include "util/Generator.hpp"
#include "util/vec_generators.hpp"

namespace {

util::Generator<int> iterateInts() {
  for (int i = 0; true; i++) {
    co_yield i;
  }
}

} // namespace

TEST(GeneratorTest, iterateIntTest) {
  util::Generator<int> intGen = iterateInts();
  EXPECT_EQ(intGen(), 0);
  EXPECT_EQ(intGen(), 1);
  EXPECT_EQ(intGen(), 2);
}

namespace {

util::Generator<int> iterateDoubles() {
  util::Generator<int> intGen = iterateInts();
  while (!intGen.isDone()) {
    co_yield 2 * intGen();
  }
}

} // namespace

TEST(GeneratorTest, iterateDoubles) {
  util::Generator<int> intGen = iterateDoubles();
  EXPECT_EQ(intGen(), 0);
  EXPECT_EQ(intGen(), 2);
  EXPECT_EQ(intGen(), 4);
}

namespace {

util::Generator<int> sharedPtrPassthrough(std::shared_ptr<int> ptr) {
  co_return;
}

} // namespace

TEST(GeneratorTest, CleanupTest) {
  const std::shared_ptr<int> ptr = std::make_shared<int>(0);
  EXPECT_EQ(ptr.use_count(), 1);

  {
    const util::Generator<int> genHolder = sharedPtrPassthrough(ptr);
    EXPECT_EQ(ptr.use_count(), 2);
  }

  EXPECT_EQ(ptr.use_count(), 1);
}

namespace {

util::Generator<int> upToThree() {
  co_yield 1;
  co_yield 2;
  co_yield 3;
}

} // namespace

TEST(GeneratorTest, UpToThree) {
  util::Generator<int> intGen = upToThree();
  EXPECT_EQ(intGen(), 1);
  EXPECT_EQ(intGen(), 2);
  EXPECT_EQ(intGen(), 3);
  EXPECT_TRUE(intGen.isDone());
}

TEST(GeneratorTest, vecSimpleGroups) {
  std::array<int, 5> arr{1, 1, 3, 3, 1};
  util::Generator<std::span<int>> groups =
      util::vec::genGroups(std::span<int>{arr});
  std::span<int> curr = groups();
  EXPECT_EQ(curr.size(), 2);
  EXPECT_TRUE(std::all_of(curr.begin(), curr.end(), [](const auto& x) {
    return x == 1;
  }));
  curr = groups();
  EXPECT_EQ(curr.size(), 2);
  EXPECT_TRUE(std::all_of(curr.begin(), curr.end(), [](const auto& x) {
    return x == 3;
  }));
  curr = groups();
  EXPECT_EQ(curr.size(), 1);
  EXPECT_TRUE(std::all_of(curr.begin(), curr.end(), [](const auto& x) {
    return x == 1;
  }));
  EXPECT_TRUE(groups.isDone());
}

namespace {

util::Generator<int> throwException() {
  throw std::runtime_error{"Oops"};
  co_return;
}

} // namespace

TEST(GeneratorTest, exceptionTest) {
  util::Generator<int> exceptionGenerator = throwException();

  EXPECT_THROW(exceptionGenerator(), std::runtime_error);
  EXPECT_TRUE(exceptionGenerator.isDone());
}
