#include "render/VulkanSurface.hpp"

#include <stdexcept>
#include <utility>

namespace blocks::render {

VulkanSurface::VulkanSurface(VulkanInstance& instance, glfw::Window window)
    : window_(std::move(window)), surface_(nullptr, nullptr) {
  VkSurfaceKHR surface;
  VkResult result = glfwCreateWindowSurface(
      instance.getRawInstance(), window_.getRawWindow(), nullptr, &surface);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create window surface"};
  }

  surface_ =
      VulkanUniqueHandle<VkSurfaceKHR>{surface, instance.getRawInstance()};
}

} // namespace blocks::render
