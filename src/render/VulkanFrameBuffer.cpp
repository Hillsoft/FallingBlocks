#include "render/VulkanFrameBuffer.hpp"

#include <stdexcept>

namespace blocks::render {

VulkanFrameBuffer::VulkanFrameBuffer(
    VulkanGraphicsDevice& device,
    VulkanRenderPass& renderPass,
    VulkanImageView& imageView,
    VkExtent2D extent)
    : extent_(extent), frameBuffer_(nullptr, nullptr) {
  VkImageView myImageView = imageView.getRawImageView();

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass.getRawRenderPass();
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = &myImageView;
  framebufferInfo.width = extent.width;
  framebufferInfo.height = extent.height;
  framebufferInfo.layers = 1;

  VkFramebuffer frameBuffer;
  VkResult result = vkCreateFramebuffer(
      device.getRawDevice(), &framebufferInfo, nullptr, &frameBuffer);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create frame buffer"};
  }

  frameBuffer_ =
      VulkanUniqueHandle<VkFramebuffer>{frameBuffer, device.getRawDevice()};
}

} // namespace blocks::render
