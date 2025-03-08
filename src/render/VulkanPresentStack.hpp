#pragma once

#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanFrameBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanSurface.hpp"
#include "render/VulkanSwapChain.hpp"

namespace tetris::render {

class VulkanPresentStack {
 public:
  VulkanPresentStack(
      VulkanGraphicsDevice& device,
      VulkanSurface& surface,
      VulkanRenderPass& renderPass);

  uint32_t getNextImageIndex(VulkanSemaphore* semaphore, VulkanFence* fence) {
    return swapChain_.getNextImageIndex(semaphore, fence);
  }

  VulkanFrameBuffer& getFrameBuffer(uint32_t imageIndex) {
    return frameBuffer_[imageIndex];
  }

  void present(uint32_t imageIndex, VulkanSemaphore* waitSemaphore) {
    swapChain_.present(imageIndex, waitSemaphore);
  }

 private:
  VulkanSwapChain swapChain_;
  std::vector<VulkanImageView> imageViews_;
  std::vector<VulkanFrameBuffer> frameBuffer_;
};

} // namespace tetris::render
