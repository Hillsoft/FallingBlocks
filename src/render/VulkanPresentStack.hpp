#pragma once

#include <cstdint>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanSwapChain.hpp"
#include "render/glfw_wrapper/Window.hpp"
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
      VkInstance instance,
      VulkanGraphicsDevice& device,
      glfw::Window window,
      VkRenderPass renderPass);

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

  glfw::Window& getWindow() { return window_; }
  const glfw::Window& getWindow() const { return window_; }

  void reset();

 private:
  struct SwapChainData {
    SwapChainData(
        VulkanGraphicsDevice& device,
        GLFWwindow* window,
        VkSurfaceKHR surface,
        VkRenderPass renderPass);

    VulkanSwapChain swapChain;
    std::vector<vulkan::UniqueHandle<VkImageView>> imageViews;
    std::vector<vulkan::UniqueHandle<VkFramebuffer>> frameBuffer;
  };

  glfw::Window window_;
  vulkan::UniqueHandle<VkSurfaceKHR> surface_;
  VkRenderPass renderPass_;
  util::Resettable<SwapChainData> swapChainData_;
  VulkanGraphicsDevice* device_;
};

} // namespace blocks::render
