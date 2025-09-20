#define GLFW_EXPOSE_NATIVE_WIN32
#include "render/Window.hpp"

#include <Windows.h>
#include <libloaderapi.h>
#include <functional>
#include <type_traits>
#include <utility>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/glfw_wrapper/Window.hpp"

namespace blocks::render {

namespace {

glfw::Window makeWindow(
    int width,
    int height,
    const char* title,
    std::function<void(int, int)> resizeHandler) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfw::Window window{width, height, title};
  window.setResizeHandler(std::move(resizeHandler));

  HICON icon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCEW(103));
  HWND hWnd = glfwGetWin32Window(window.getRawWindow());
  SendMessage(hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
  SendMessage(hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));

  return window;
}

} // namespace

Window::Window(
    VulkanInstance& instance,
    VulkanGraphicsDevice& device,
    VkRenderPass renderPass,
    int width,
    int height,
    const char* title)
    : presentStack_(
          instance.getRawInstance(),
          device,
          makeWindow(width, height, title, [&](int, int) { onResize(); }),
          renderPass),
      requiresReset_(false) {}

void Window::close() {
  glfwSetWindowShouldClose(presentStack_.getWindow().getRawWindow(), GLFW_TRUE);
}

bool Window::shouldClose() const {
  return presentStack_.getWindow().shouldClose();
}

std::pair<int, int> Window::getCurrentWindowSize() const {
  return presentStack_.getWindow().getCurrentWindowSize();
}

VkExtent2D Window::getCurrentWindowExtent() const {
  return presentStack_.extent();
}

void Window::toggleFullScreen() {
  GLFWwindow* rawWindow = presentStack_.getWindow().getRawWindow();

  if (lastWindowedExtent_.width == 0) {
    // change to fullscreen
    lastWindowedExtent_ = getCurrentWindowExtent();
    glfwGetWindowPos(
        rawWindow, &lastWindowedXPosition_, &lastWindowedYPosition_);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(
        rawWindow,
        monitor,
        0,
        0,
        vidmode->width,
        vidmode->height,
        GLFW_DONT_CARE);
  } else {
    glfwSetWindowMonitor(
        rawWindow,
        nullptr,
        lastWindowedXPosition_,
        lastWindowedYPosition_,
        lastWindowedExtent_.width,
        lastWindowedExtent_.height,
        GLFW_DONT_CARE);

    lastWindowedExtent_ = {0, 0};
  }
}

void Window::resetSwapChain() {
  std::pair<int, int> windowSize = getCurrentWindowSize();
  if (!(windowSize.first == 0 || windowSize.second == 0)) {
    presentStack_.reset();
    requiresReset_ = false;
  }
}

bool Window::requiresReset() const {
  return requiresReset_;
}

bool Window::isDrawable() const {
  return !requiresReset_;
}

void Window::onResize() {
  requiresReset_ = true;
}

} // namespace blocks::render
