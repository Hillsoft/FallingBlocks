#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/glfw_wrapper/Window.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanSurface {
 public:
  VulkanSurface(VulkanInstance& instance, glfw::Window window);

  glfw::Window& window() { return window_; }
  const glfw::Window& window() const { return window_; }

  VkSurfaceKHR getRawSurface() { return surface_.get(); }

 private:
  glfw::Window window_;
  vulkan::UniqueHandle<VkSurfaceKHR> surface_;
};

} // namespace blocks::render
