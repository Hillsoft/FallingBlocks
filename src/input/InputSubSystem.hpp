#pragma once

#include "render/glfw_wrapper/Window.hpp"

namespace blocks::input {

class InputSubSystem {
 public:
  InputSubSystem(render::glfw::Window& window);

  InputSubSystem(const InputSubSystem& other) = delete;
  InputSubSystem& operator=(const InputSubSystem& other) = delete;

  InputSubSystem(InputSubSystem&& other) = delete;
  InputSubSystem& operator=(InputSubSystem&& other) = delete;

  ~InputSubSystem();

 private:
  void handleKeyEvent(int key, int scancode, int action, int mods);

  render::glfw::Window* window_;
};

} // namespace blocks::input
