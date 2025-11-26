#pragma once

#include "math/vec.hpp"
#include "render/glfw_wrapper/Window.hpp"
#include "util/NotNull.hpp"
#include "util/Registry.hpp"

namespace blocks::input {

class InputHandler;

class InputSubSystem : public util::Registry<InputHandler, InputSubSystem> {
 public:
  InputSubSystem(render::glfw::Window& window);

  ~InputSubSystem();

 private:
  void handleKeyEvent(int key, int scancode, int action, int mods);
  void handleCursorPos(double xpos, double ypos);
  void handleMouseEvent(int button, int action, int mods);

  util::NotNullPtr<render::glfw::Window> window_;
  math::Vec2 mousePos_;
};

} // namespace blocks::input
