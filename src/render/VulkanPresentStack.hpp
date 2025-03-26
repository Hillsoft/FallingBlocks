#pragma once

#include <cstdint>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanSurface.hpp"
#include "render/VulkanSwapChain.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/debug.hpp"
#include "util/resettable.hpp"

namespace blocks::render {

class VulkanPresentStack {
 private:
  struct FrameDataCreationTag {};

 public:
  class FrameData {
   public:
    FrameData(
        FrameDataCreationTag /* tag */,
        uint32_t imageIndex,
        VulkanPresentStack* owner);
    FrameData(FrameDataCreationTag /* tag */);

    bool refreshRequired() const { return refreshSwapChainRequired_; }

    VkFramebuffer getFrameBuffer() const {
      DEBUG_ASSERT(!refreshSwapChainRequired_);
      return owner_->getFrameBuffer(imageIndex_);
    }

    void present(VkSemaphore waitSemaphore) const {
      DEBUG_ASSERT(!refreshSwapChainRequired_);
      return owner_->present(imageIndex_, waitSemaphore);
    }

   private:
    bool refreshSwapChainRequired_;
    uint32_t imageIndex_;
    VulkanPresentStack* owner_;
  };

  VulkanPresentStack(
      VulkanGraphicsDevice& device,
      VulkanSurface& surface,
      VulkanRenderPass& renderPass);

  FrameData getNextImageIndex(VkSemaphore semaphore, VkFence fence);

  VkFramebuffer getFrameBuffer(uint32_t imageIndex) {
    return swapChainData_->frameBuffer[imageIndex].get();
  }

  void present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
    swapChainData_->swapChain.present(imageIndex, waitSemaphore);
  }

  VkExtent2D extent() const {
    return swapChainData_->swapChain.getSwapchainExtent();
  }

  void reset();

 private:
  struct SwapChainData {
    SwapChainData(
        VulkanGraphicsDevice& device,
        VulkanSurface& surface,
        VulkanRenderPass& renderPass);

    VulkanSwapChain swapChain;
    std::vector<VulkanImageView> imageViews;
    std::vector<vulkan::UniqueHandle<VkFramebuffer>> frameBuffer;
  };

  util::Resettable<SwapChainData> swapChainData_;
  VulkanGraphicsDevice* device_;
  VulkanSurface* surface_;
  VulkanRenderPass* renderPass_;
};

} // namespace blocks::render
