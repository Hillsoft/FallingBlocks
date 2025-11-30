#pragma once

#include "math/vec.hpp"
#include "render/glfw_wrapper/Window.hpp"
#include "util/NotNull.hpp"
#include "util/Registry.hpp"

namespace blocks::input {

class InputHandler;

class InputRegistry : public util::Registry<InputHandler, InputRegistry> {
 public:
  InputRegistry() = default;

  void handleKeyEvent(int key, int scancode, int action, int mods);
  void handleCursorPos(math::Vec2 mousePos);
  void handleMouseEvent(math::Vec2 mousePos, int action);
};

class InputSubSystem {
 public:
  explicit InputSubSystem(render::glfw::Window& window);

  ~InputSubSystem();

  InputSubSystem(const InputSubSystem& other) = delete;
  InputSubSystem& operator=(const InputSubSystem& other) = delete;

  InputSubSystem(InputSubSystem&& other) = delete;
  InputSubSystem& operator=(InputSubSystem&& other) = delete;

  void setActiveRegistry(InputRegistry* registry);
  void unsetActiveRegistry(InputRegistry* registry);

 private:
  void handleKeyEvent(int key, int scancode, int action, int mods);
  void handleCursorPos(double xpos, double ypos);
  void handleMouseEvent(int button, int action, int mods);

  InputRegistry* activeRegistry_ = nullptr;
  util::NotNullPtr<render::glfw::Window> window_;
  math::Vec2 mousePos_;
};

} // namespace blocks::input
