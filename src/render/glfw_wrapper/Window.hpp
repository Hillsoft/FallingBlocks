#pragma once

#include <functional>
#include <GLFW/glfw3.h>

namespace blocks::render::glfw {

class Window {
 public:
  Window(int width, int height, const char* title);
  ~Window();

  Window(const Window& other) = delete;
  Window& operator=(const Window& other) = delete;

  Window(Window&& other) noexcept;
  Window& operator=(Window&& other) noexcept;

  bool shouldClose() const;

  std::pair<int, int> getCurrentWindowSize() const;

  void setResizeHandler(
      std::function<void(int width, int height)> resizeHandler) {
    resizeHandler_ = std::move(resizeHandler);
  }

  GLFWwindow* getRawWindow() { return window_; }

 private:
  GLFWwindow* window_;
  std::function<void(int width, int height)> resizeHandler_;

  static void frameBufferResizeCallback(
      GLFWwindow* window, int width, int height);
};

} // namespace blocks::render::glfw
