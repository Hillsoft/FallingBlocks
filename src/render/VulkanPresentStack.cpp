#include "render/VulkanPresentStack.hpp"

#include <iostream>
#include <stdexcept>

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

uint32_t VulkanPresentStack::getNextImageIndex(
    VulkanSemaphore* semaphore, VulkanFence* fence) {
  std::optional<uint32_t> nextImageIndex =
      swapChainData_->swapChain.getNextImageIndex(semaphore, fence);
  if (nextImageIndex.has_value()) {
    return *nextImageIndex;
  }

  std::cout << "Resetting swap chain\n";
  vkDeviceWaitIdle(device_->getRawDevice());
  swapChainData_.reset(*device_, *surface_, *renderPass_);

  nextImageIndex =
      swapChainData_->swapChain.getNextImageIndex(semaphore, fence);
  if (nextImageIndex.has_value()) {
    return *nextImageIndex;
  }

  throw std::runtime_error{
      "Recreating swapchain failed to resolve out of data issue"};
}

} // namespace blocks::render
