#pragma once

#include "util/raii_helpers.hpp"

namespace blocks::input {

class InputSubSystem;

class InputHandler : private util::no_copy_move {
 public:
  InputHandler(InputSubSystem& inputSystem);

  virtual ~InputHandler();

  virtual void onKeyPress(int keyCode) {}
  virtual void onKeyRelease(int keyCode) {}

 private:
  InputSubSystem* inputSystem_;
};

} // namespace blocks::input
