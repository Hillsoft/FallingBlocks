#pragma once

#include <utility>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/VulkanPresentStack.hpp"

namespace blocks::render {

class Window {
 public:
  Window(
      VulkanInstance& instance,
      VulkanGraphicsDevice& device,
      VkRenderPass renderPass,
      int width,
      int height,
      const char* title);

  bool shouldClose() const;
  std::pair<int, int> getCurrentWindowSize() const;

 private:
  VulkanPresentStack presentStack_;

 public:
  friend class GLFWApplication;
};

} // namespace blocks::render
