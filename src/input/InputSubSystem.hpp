#pragma once

#include "render/glfw_wrapper/Window.hpp"
#include "util/Registry.hpp"

namespace blocks::input {

class InputHandler;

class InputSubSystem : public util::Registry<InputHandler, InputSubSystem> {
 public:
  InputSubSystem(render::glfw::Window& window);

  ~InputSubSystem();

 private:
  void handleKeyEvent(int key, int scancode, int action, int mods);

  render::glfw::Window* window_;
};

} // namespace blocks::input
