#include "render/VulkanSurface.hpp"

#include <stdexcept>
#include <type_traits>
#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/glfw_wrapper/Window.hpp"
#include "render/vulkan/UniqueHandle.hpp"

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
      vulkan::UniqueHandle<VkSurfaceKHR>{surface, instance.getRawInstance()};
}

} // namespace blocks::render
