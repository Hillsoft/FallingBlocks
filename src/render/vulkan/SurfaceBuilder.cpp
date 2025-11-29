#include "render/vulkan/SurfaceBuilder.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

vulkan::UniqueHandle<VkSurfaceKHR> createSurfaceForWindow(
    VkInstance instance, GLFWwindow* window) {
  VkSurfaceKHR surface = nullptr;
  const VkResult result =
      glfwCreateWindowSurface(instance, window, nullptr, &surface);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create window surface"};
  }

  return vulkan::UniqueHandle<VkSurfaceKHR>{surface, instance};
}

} // namespace blocks::render::vulkan
