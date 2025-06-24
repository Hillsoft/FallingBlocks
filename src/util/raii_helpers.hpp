#pragma once

namespace util {

class no_copy {
 public:
  no_copy() = default;

  no_copy(const no_copy& other) = delete;
  no_copy& operator=(const no_copy& other) = delete;

  no_copy(no_copy&& other) = default;
  no_copy& operator=(no_copy&& other) = default;

  ~no_copy() = default;
};

class no_move {
 public:
  no_move() = default;

  no_move(const no_move& other) = default;
  no_move& operator=(const no_move& other) = default;

  no_move(no_move&& other) = delete;
  no_move& operator=(no_move&& other) = delete;

  ~no_move() = default;
};

class no_copy_move : private no_copy, no_move {};

} // namespace util
