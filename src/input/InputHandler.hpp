#pragma once

#include "math/vec.hpp"
#include "util/Registry.hpp"

namespace blocks::input {

class InputSubSystem;

class InputHandler : public util::RegistryItem<InputSubSystem, InputHandler> {
 public:
  explicit InputHandler(InputSubSystem& inputSystem);

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
