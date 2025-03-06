#pragma once

#include <GLFW/glfw3.h>

namespace tetris::render::glfw {

class Window {
 public:
  Window(int width, int height, const char* title);
  ~Window();

  Window(const Window& other) = delete;
  Window& operator=(const Window& other) = delete;

  Window(Window&& other) noexcept;
  Window& operator=(Window&& other) noexcept;

  bool shouldClose() const;

  GLFWwindow* getRawWindow() { return window_; }

 private:
  GLFWwindow* window_;
};

} // namespace tetris::render::glfw
