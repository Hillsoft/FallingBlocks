#pragma once

#include <vector>
#include "render/glfw_wrapper/Window.hpp"

namespace blocks::input {

class InputHandler;

class InputSubSystem {
 public:
  InputSubSystem(render::glfw::Window& window);

  InputSubSystem(const InputSubSystem& other) = delete;
  InputSubSystem& operator=(const InputSubSystem& other) = delete;

  InputSubSystem(InputSubSystem&& other) = delete;
  InputSubSystem& operator=(InputSubSystem&& other) = delete;

  ~InputSubSystem();

  void registerHandler(InputHandler& handler);
  void unregisterHandler(InputHandler& handler);

 private:
  void handleKeyEvent(int key, int scancode, int action, int mods);

  render::glfw::Window* window_;
  std::vector<InputHandler*> handlers_;
};

} // namespace blocks::input
