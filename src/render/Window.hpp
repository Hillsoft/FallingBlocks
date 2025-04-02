#pragma once

#include <utility>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/VulkanPresentStack.hpp"

namespace blocks::render {

class RenderSubSystem;

class Window {
 public:
  Window(
      VulkanInstance& instance,
      VulkanGraphicsDevice& device,
      VkRenderPass renderPass,
      int width,
      int height,
      const char* title);

  Window(const Window& other) = delete;
  Window& operator=(const Window& other) = delete;
  Window(Window&& other) = delete;
  Window& operator=(Window&& other) = delete;

  bool shouldClose() const;
  std::pair<int, int> getCurrentWindowSize() const;
  VkExtent2D getCurrentWindowExtent() const;

  void resetSwapChain();

  VulkanPresentStack& getPresentStack() { return presentStack_; }

  bool requiresReset() const;
  bool isDrawable() const;

 private:
  void onResize();

  VulkanPresentStack presentStack_;
  bool requiresReset_;
};

} // namespace blocks::render
