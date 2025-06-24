#pragma once

#include <vector>
#include "render/glfw_wrapper/Window.hpp"
#include "util/raii_helpers.hpp"

namespace blocks::input {

class InputHandler;

class InputSubSystem : private util::no_copy_move {
 public:
  InputSubSystem(render::glfw::Window& window);

  ~InputSubSystem();

  void registerHandler(InputHandler& handler);
  void unregisterHandler(InputHandler& handler);

 private:
  void handleKeyEvent(int key, int scancode, int action, int mods);

  render::glfw::Window* window_;
  std::vector<InputHandler*> handlers_;
};

} // namespace blocks::input
