#pragma once

#include "render/VulkanInstance.hpp"
#include "render/glfw_wrapper/Window.hpp"

namespace blocks::render {

class VulkanSurface {
 public:
  VulkanSurface(VulkanInstance& instance, glfw::Window window);
  ~VulkanSurface();

  VulkanSurface(const VulkanSurface& other) = delete;
  VulkanSurface& operator=(const VulkanSurface& other) = delete;

  VulkanSurface(VulkanSurface&& other) noexcept;
  VulkanSurface& operator=(VulkanSurface&& other) noexcept;

  glfw::Window& window() { return window_; }
  const glfw::Window& window() const { return window_; }

  VkSurfaceKHR getRawSurface() { return surface_; }

 private:
  VulkanInstance* instance_;
  glfw::Window window_;
  VkSurfaceKHR surface_;
};

} // namespace blocks::render
