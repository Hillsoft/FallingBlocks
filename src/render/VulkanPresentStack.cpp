#include "render/VulkanPresentStack.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanSurface.hpp"
#include "render/vulkan/FrameBufferBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/resettable.hpp"

namespace blocks::render {

namespace {

std::vector<vulkan::UniqueHandle<VkFramebuffer>> makeFrameBuffers(
    VulkanGraphicsDevice& device,
    VkRenderPass renderPass,
    std::vector<VulkanImageView>& imageViews,
    VkExtent2D extent) {
  std::vector<vulkan::UniqueHandle<VkFramebuffer>> result;
  result.reserve(imageViews.size());

  for (auto& v : imageViews) {
    VkImageView rawView = v.getRawImageView();
    result.emplace_back(
        vulkan::FrameBufferBuilder(
            renderPass, std::span{&rawView, 1}, extent, 1)
            .build(device.getRawDevice()));
  }

  return result;
}

} // namespace

VulkanPresentStack::VulkanPresentStack(
    VulkanGraphicsDevice& device,
    VulkanSurface&& surface,
    VkRenderPass renderPass)
    : surface_(std::move(surface)),
      renderPass_(renderPass),
      swapChainData_(device, surface_, renderPass),
      device_(&device) {}

VulkanPresentStack::SwapChainData::SwapChainData(
    VulkanGraphicsDevice& device,
    VulkanSurface& surface,
    VkRenderPass renderPass)
    : swapChain(surface, device),
      imageViews(swapChain.getImageViews()),
      frameBuffer(makeFrameBuffers(
          device, renderPass, imageViews, swapChain.getSwapchainExtent())) {}

VulkanPresentStack::FrameData VulkanPresentStack::getNextImageIndex(
    VkSemaphore semaphore, VkFence fence) {
  std::optional<uint32_t> nextImageIndex =
      swapChainData_->swapChain.getNextImageIndex(semaphore, fence);
  if (nextImageIndex.has_value()) {
    return FrameData{FrameDataCreationTag{}, *nextImageIndex, this};
  } else {
    return FrameData{FrameDataCreationTag{}};
  }
}

void VulkanPresentStack::reset() {
  std::cout << "Resetting swap chain\n";
  vkDeviceWaitIdle(device_->getRawDevice());
  swapChainData_.reset(*device_, surface_, renderPass_);
}

VulkanPresentStack::FrameData::FrameData(
    FrameDataCreationTag /* tag */,
    uint32_t imageIndex,
    VulkanPresentStack* owner)
    : refreshSwapChainRequired_(false),
      imageIndex_(imageIndex),
      owner_(owner) {}

VulkanPresentStack::FrameData::FrameData(FrameDataCreationTag /* tag */)
    : refreshSwapChainRequired_(true), imageIndex_(0), owner_(nullptr) {}

} // namespace blocks::render
