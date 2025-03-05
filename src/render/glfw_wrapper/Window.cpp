#include "render/glfw_wrapper/Window.h"

#include <stdexcept>

#include "util/string.hpp"

namespace tetris::render::glfw {

Window::Window(int width, int height, const char* title)
    : window_(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
  if (window_ == nullptr) {
    throw std::runtime_error{
        util::toString("Failed to create window '", title, "'")};
  }
}

Window::~Window() {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
}

Window::Window(Window&& other) noexcept : window_(other.window_) {
  other.window_ = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }

  window_ = other.window_;
  other.window_ = nullptr;
  return *this;
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(window_);
}

} // namespace tetris::render::glfw
