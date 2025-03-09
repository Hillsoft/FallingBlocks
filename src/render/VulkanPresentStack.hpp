#pragma once

#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanFrameBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanSurface.hpp"
#include "render/VulkanSwapChain.hpp"
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

    VulkanFrameBuffer& getFrameBuffer() const {
      return owner_->getFrameBuffer(imageIndex_);
    }

    void present(VulkanSemaphore* waitSemaphore) const {
      return owner_->present(imageIndex_, waitSemaphore);
    }

   private:
    uint32_t imageIndex_;
    VulkanPresentStack* owner_;
  };

  VulkanPresentStack(
      VulkanGraphicsDevice& device,
      VulkanSurface& surface,
      VulkanRenderPass& renderPass);

  FrameData getNextImageIndex(VulkanSemaphore* semaphore, VulkanFence* fence);

  VulkanFrameBuffer& getFrameBuffer(uint32_t imageIndex) {
    return swapChainData_->frameBuffer[imageIndex];
  }

  void present(uint32_t imageIndex, VulkanSemaphore* waitSemaphore) {
    swapChainData_->swapChain.present(imageIndex, waitSemaphore);
  }

 private:
  struct SwapChainData {
    SwapChainData(
        VulkanGraphicsDevice& device,
        VulkanSurface& surface,
        VulkanRenderPass& renderPass);

    VulkanSwapChain swapChain;
    std::vector<VulkanImageView> imageViews;
    std::vector<VulkanFrameBuffer> frameBuffer;
  };

  util::Resettable<SwapChainData> swapChainData_;
  VulkanGraphicsDevice* device_;
  VulkanSurface* surface_;
  VulkanRenderPass* renderPass_;
};

} // namespace blocks::render
