#pragma once

#include <utility>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
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

  ~Window() = default;

  void close();
  [[nodiscard]] bool shouldClose() const;
  [[nodiscard]] std::pair<int, int> getCurrentWindowSize() const;
  [[nodiscard]] VkExtent2D getCurrentWindowExtent() const;

  void toggleFullScreen();

  void resetSwapChain();

  [[nodiscard]] VulkanPresentStack& getPresentStack() { return presentStack_; }

  [[nodiscard]] bool requiresReset() const;
  [[nodiscard]] bool isDrawable() const;

 private:
  void onResize();

  VulkanPresentStack presentStack_;
  bool requiresReset_{false};
  VkExtent2D lastWindowedExtent_{0, 0};
  int lastWindowedXPosition_{0};
  int lastWindowedYPosition_{0};
};

} // namespace blocks::render
