#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanFence.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanSemaphore.hpp"
#include "render/VulkanSurface.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanSwapChain {
 public:
  VulkanSwapChain(VulkanSurface& surface, VulkanGraphicsDevice& graphicsDevice);

  std::vector<VulkanImageView> getImageViews() const;

  // nullopt indicates out of date swapchain, recreation required
  std::optional<uint32_t> getNextImageIndex(
      VulkanSemaphore* semaphore, VulkanFence* fence);

  VkExtent2D getSwapchainExtent() const { return extent_; }

  void present(uint32_t imageIndex, VulkanSemaphore* waitSemaphore);

 private:
  VulkanGraphicsDevice* graphicsDevice_;
  VulkanUniqueHandle<VkSwapchainKHR> swapChain_;
  VkExtent2D extent_;
  VkQueue queue_;
  std::vector<VkImage> swapChainImages_;
};

} // namespace blocks::render
