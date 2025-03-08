#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanFence.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanSemaphore.hpp"
#include "render/VulkanSurface.hpp"

namespace tetris::render {

class VulkanSwapChain {
 public:
  VulkanSwapChain(VulkanSurface& surface, VulkanGraphicsDevice& graphicsDevice);
  ~VulkanSwapChain();

  VulkanSwapChain(const VulkanSwapChain& other) = delete;
  VulkanSwapChain& operator=(const VulkanSwapChain& other) = delete;

  VulkanSwapChain(VulkanSwapChain&& other) noexcept;
  VulkanSwapChain& operator=(VulkanSwapChain&& other) noexcept;

  std::vector<VulkanImageView> getImageViews() const;

  uint32_t getNextImageIndex(VulkanSemaphore* semaphore, VulkanFence* fence);

  VkExtent2D getSwapchainExtent() const { return extent_; }

  void present(uint32_t imageIndex, VulkanSemaphore* waitSemaphore);

 private:
  VulkanGraphicsDevice* graphicsDevice_;
  VkSwapchainKHR swapChain_;
  VkExtent2D extent_;
  VkQueue queue_;
  std::vector<VkImage> swapChainImages_;
};

} // namespace tetris::render
