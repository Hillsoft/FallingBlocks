#include "render/glfw_wrapper/Window.hpp"

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <GLFW/glfw3.h>

#include "util/string.hpp"

namespace blocks::render::glfw {

Window::Window(int width, int height, const char* title)
    : window_(glfwCreateWindow(width, height, title, nullptr, nullptr)),
      resizeHandler_([](int width, int height) {}) {
  if (window_ == nullptr) {
    throw std::runtime_error{
        util::toString("Failed to create window '", title, "'")};
  }

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, frameBufferResizeCallback);
}

Window::~Window() {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
}

Window::Window(Window&& other) noexcept
    : window_(other.window_), resizeHandler_(std::move(other.resizeHandler_)) {
  other.window_ = nullptr;

  glfwSetWindowUserPointer(window_, this);
}

Window& Window::operator=(Window&& other) noexcept {
  std::swap(window_, other.window_);
  std::swap(resizeHandler_, other.resizeHandler_);

  glfwSetWindowUserPointer(window_, this);
  glfwSetWindowUserPointer(other.window_, &other);

  return *this;
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(window_);
}

std::pair<int, int> Window::getCurrentWindowSize() const {
  std::pair<int, int> result{0, 0};
  glfwGetFramebufferSize(window_, &result.first, &result.second);
  return result;
}

void Window::frameBufferResizeCallback(
    GLFWwindow* window, int width, int height) {
  auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  appWindow->resizeHandler_(width, height);
}

} // namespace blocks::render::glfw
