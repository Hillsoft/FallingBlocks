#pragma once

#include "util/Registry.hpp"

namespace blocks::input {

class InputSubSystem;

class InputHandler : public util::RegistryItem<InputSubSystem, InputHandler> {
 public:
  InputHandler(InputSubSystem& inputSystem);

  virtual ~InputHandler() {}

  virtual void onKeyPress(int keyCode) {}
  virtual void onKeyRelease(int keyCode) {}
};

} // namespace blocks::input
