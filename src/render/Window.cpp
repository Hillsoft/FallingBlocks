#include "render/Window.hpp"

#include <functional>
#include <type_traits>
#include <utility>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/VulkanSurface.hpp"
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
          device,
          VulkanSurface{
              instance,
              makeWindow(width, height, title, [&](int, int) { onResize(); })},
          renderPass),
      requiresReset_(false) {}

bool Window::shouldClose() const {
  return presentStack_.getSurface().window().shouldClose();
}

std::pair<int, int> Window::getCurrentWindowSize() const {
  return presentStack_.getSurface().window().getCurrentWindowSize();
}

VkExtent2D Window::getCurrentWindowExtent() const {
  return presentStack_.extent();
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
