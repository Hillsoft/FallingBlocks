#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include <GLFW/glfw3.h>

namespace blocks::render::glfw {

class Window {
 public:
  Window(int width, int height, const char* title);
  ~Window();

  Window(Window&& other) noexcept;
  Window& operator=(Window&& other) noexcept;

  Window(const Window& other) = delete;
  Window& operator=(const Window& other) = delete;

  [[nodiscard]] bool shouldClose() const;

  [[nodiscard]] std::pair<int, int> getCurrentWindowSize() const;

  void setResizeHandler(
      std::function<void(int width, int height)> resizeHandler) {
    resizeHandler_ = std::move(resizeHandler);
  }

  void setKeyEventHandler(
      std::function<void(int key, int scancode, int action, int mods)>
          keyHandler) {
    keyEventHandler_ = std::move(keyHandler);
  }

  void setMouseButtonHandler(
      std::function<void(int button, int action, int mods)>
          mouseButtonHandler) {
    mouseButtonHandler_ = std::move(mouseButtonHandler);
  }

  void setCursorPosHandler(
      std::function<void(double xpos, double ypos)> cursorPosHandler) {
    cursorPosHandler_ = std::move(cursorPosHandler);
  }

  GLFWwindow* getRawWindow() { return window_; }

 private:
  GLFWwindow* window_;
  std::function<void(int width, int height)> resizeHandler_;
  std::function<void(int key, int scancode, int action, int mods)>
      keyEventHandler_;
  std::function<void(int button, int action, int mods)> mouseButtonHandler_;
  std::function<void(double xpos, double ypos)> cursorPosHandler_;

  static void frameBufferResizeCallback(
      GLFWwindow* window, int width, int height);

  static void keyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  static void mouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};

} // namespace blocks::render::glfw
