#include "render/VulkanPresentStack.hpp"

namespace tetris::render {

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
    : swapChain_(surface, device),
      imageViews_(swapChain_.getImageViews()),
      frameBuffer_(makeFrameBuffers(
          device, renderPass, imageViews_, swapChain_.getSwapchainExtent())) {}

} // namespace tetris::render
