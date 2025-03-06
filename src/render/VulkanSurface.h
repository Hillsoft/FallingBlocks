#pragma once

#include "render/VulkanInstance.h"
#include "render/glfw_wrapper/Window.h"

namespace tetris::render {

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

 private:
  VulkanInstance* instance_;
  glfw::Window window_;
  VkSurfaceKHR surface_;
};

} // namespace tetris::render
