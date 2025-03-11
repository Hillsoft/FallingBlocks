#include "render/VulkanImageView.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanImageView::VulkanImageView(
    VulkanGraphicsDevice& device, VkImage image, VkFormat imageFormat)
    : imageView_(nullptr, nullptr) {
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = image;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = imageFormat;
  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  VkResult result = vkCreateImageView(
      device.getRawDevice(), &createInfo, nullptr, &imageView);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create image view"};
  }

  imageView_ =
      vulkan::UniqueHandle<VkImageView>{imageView, device.getRawDevice()};
}

} // namespace blocks::render
