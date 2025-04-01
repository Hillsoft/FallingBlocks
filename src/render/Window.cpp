#include "render/Window.hpp"

#include <utility>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/VulkanSurface.hpp"
#include "render/glfw_wrapper/Window.hpp"

namespace blocks::render {

namespace {

glfw::Window makeWindow(int width, int height, const char* title) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfw::Window window{width, height, title};

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
          VulkanSurface{instance, makeWindow(width, height, title)},
          renderPass) {}

bool Window::shouldClose() const {
  return presentStack_.getSurface().window().shouldClose();
}

std::pair<int, int> Window::getCurrentWindowSize() const {
  return presentStack_.getSurface().window().getCurrentWindowSize();
}

} // namespace blocks::render
