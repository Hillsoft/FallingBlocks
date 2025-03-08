#include "render/VulkanFrameBuffer.hpp"

#include <stdexcept>

namespace tetris::render {

VulkanFrameBuffer::VulkanFrameBuffer(
    VulkanGraphicsDevice& device,
    VulkanRenderPass& renderPass,
    VulkanImageView& imageView,
    VkExtent2D extent)
    : device_(&device), extent_(extent), frameBuffer_(nullptr) {
  VkImageView myImageView = imageView.getRawImageView();

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass.getRawRenderPass();
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = &myImageView;
  framebufferInfo.width = extent.width;
  framebufferInfo.height = extent.height;
  framebufferInfo.layers = 1;

  VkResult result = vkCreateFramebuffer(
      device.getRawDevice(), &framebufferInfo, nullptr, &frameBuffer_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create frame buffer"};
  }
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  if (frameBuffer_ != nullptr) {
    vkDestroyFramebuffer(device_->getRawDevice(), frameBuffer_, nullptr);
  }
}

VulkanFrameBuffer::VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept
    : device_(other.device_), frameBuffer_(other.frameBuffer_) {
  other.device_ = nullptr;
  other.frameBuffer_ = nullptr;
}

VulkanFrameBuffer& VulkanFrameBuffer::operator=(
    VulkanFrameBuffer&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(frameBuffer_, other.frameBuffer_);

  return *this;
}

} // namespace tetris::render
