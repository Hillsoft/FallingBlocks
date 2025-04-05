#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {
class VulkanSwapChain {
 public:
  VulkanSwapChain(
      GLFWwindow* window,
      VkSurfaceKHR surface,
      VulkanGraphicsDevice& graphicsDevice);

  std::vector<VulkanImageView> getImageViews() const;

  // nullopt indicates out of date swapchain, recreation required
  std::optional<uint32_t> getNextImageIndex(
      VkSemaphore semaphore, VkFence fence);

  VkExtent2D getSwapchainExtent() const { return extent_; }

  void present(uint32_t imageIndex, VkSemaphore waitSemaphore);

 private:
  VulkanGraphicsDevice* graphicsDevice_;
  vulkan::UniqueHandle<VkSwapchainKHR> swapChain_;
  VkFormat format_;
  VkExtent2D extent_;
  VkQueue queue_;
  std::vector<VkImage> swapChainImages_;
};

} // namespace blocks::render
