#include "render/VulkanImageView.hpp"

#include <stdexcept>

namespace tetris::render {

VulkanImageView::VulkanImageView(
    VulkanGraphicsDevice& device, VkImage image, VkFormat imageFormat)
    : device_(&device), imageView_(nullptr) {
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

  VkResult result = vkCreateImageView(
      device.getRawDevice(), &createInfo, nullptr, &imageView_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create image view"};
  }
}

VulkanImageView::~VulkanImageView() {
  if (imageView_ != nullptr) {
    vkDestroyImageView(device_->getRawDevice(), imageView_, nullptr);
  }
}

VulkanImageView::VulkanImageView(VulkanImageView&& other) noexcept
    : device_(other.device_), imageView_(other.imageView_) {
  other.device_ = nullptr;
  other.imageView_ = nullptr;
}

VulkanImageView& VulkanImageView::operator=(VulkanImageView&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(imageView_, other.imageView_);

  return *this;
}

} // namespace tetris::render
