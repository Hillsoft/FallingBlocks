#pragma once

#include "render/VulkanInstance.hpp"
#include "render/VulkanUniqueHandle.hpp"
#include "render/glfw_wrapper/Window.hpp"

namespace blocks::render {

class VulkanSurface {
 public:
  VulkanSurface(VulkanInstance& instance, glfw::Window window);

  glfw::Window& window() { return window_; }
  const glfw::Window& window() const { return window_; }

  VkSurfaceKHR getRawSurface() { return surface_.get(); }

 private:
  glfw::Window window_;
  VulkanUniqueHandle<VkSurfaceKHR> surface_;
};

} // namespace blocks::render
