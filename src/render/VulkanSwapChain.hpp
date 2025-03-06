#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
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

 private:
  VulkanGraphicsDevice* graphicsDevice_;
  VkSwapchainKHR swapChain_;
  std::vector<VkImage> swapChainImages_;
};

} // namespace tetris::render
