#pragma once

#include "math/vec.hpp"
#include "util/Registry.hpp"

namespace blocks::input {

class InputRegistry;

class InputHandler : public util::RegistryItem<InputRegistry, InputHandler> {
 public:
  explicit InputHandler(InputRegistry& inputSystem);

  virtual ~InputHandler() = default;

  InputHandler(const InputHandler& other) = delete;
  InputHandler& operator=(const InputHandler& other) = delete;

  InputHandler(InputHandler&& other) = delete;
  InputHandler& operator=(InputHandler&& other) = delete;

  virtual void onKeyPress(int keyCode) {}
  virtual void onKeyRelease(int keyCode) {}
  virtual void onMouseMove(math::Vec2 screenPos) {}
  virtual void onMouseDown(math::Vec2 screenPos) {}
  virtual void onMouseUp(math::Vec2 screenPos) {}
};

} // namespace blocks::input
