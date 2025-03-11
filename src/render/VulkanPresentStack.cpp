#include "render/VulkanPresentStack.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanFence.hpp"
#include "render/VulkanFrameBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanSemaphore.hpp"
#include "render/VulkanSurface.hpp"
#include "util/resettable.hpp"

namespace blocks::render {

namespace {

std::vector<VulkanFrameBuffer> makeFrameBuffers(
    VulkanGraphicsDevice& device,
    VulkanRenderPass& renderPass,
    std::vector<VulkanImageView>& imageViews,
    VkExtent2D extent) {
  std::vector<VulkanFrameBuffer> result;
  result.reserve(imageViews.size());

  for (auto& v : imageViews) {
    result.emplace_back(device, renderPass, v, extent);
  }

  return result;
}

} // namespace

VulkanPresentStack::VulkanPresentStack(
    VulkanGraphicsDevice& device,
    VulkanSurface& surface,
    VulkanRenderPass& renderPass)
    : swapChainData_(device, surface, renderPass),
      device_(&device),
      surface_(&surface),
      renderPass_(&renderPass) {}

VulkanPresentStack::SwapChainData::SwapChainData(
    VulkanGraphicsDevice& device,
    VulkanSurface& surface,
    VulkanRenderPass& renderPass)
    : swapChain(surface, device),
      imageViews(swapChain.getImageViews()),
      frameBuffer(makeFrameBuffers(
          device, renderPass, imageViews, swapChain.getSwapchainExtent())) {}

VulkanPresentStack::FrameData VulkanPresentStack::getNextImageIndex(
    VulkanSemaphore* semaphore, VulkanFence* fence) {
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
  swapChainData_.reset(*device_, *surface_, *renderPass_);
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
