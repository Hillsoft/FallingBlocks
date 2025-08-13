#include "render/vulkan/ImageViewBuilder.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

ImageViewBuilder::ImageViewBuilder(VkImage image, VkFormat imageFormat)
    : createInfo_() {
  createInfo_.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo_.image = image;
  createInfo_.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo_.format = imageFormat;
  createInfo_.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo_.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo_.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo_.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo_.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo_.subresourceRange.baseMipLevel = 0;
  createInfo_.subresourceRange.levelCount = 1;
  createInfo_.subresourceRange.baseArrayLayer = 0;
  createInfo_.subresourceRange.layerCount = 1;
}

UniqueHandle<VkImageView> ImageViewBuilder::build(VkDevice device) {
  VkImageView imageView;
  VkResult result =
      vkCreateImageView(device, &createInfo_, nullptr, &imageView);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create image view"};
  }
  return UniqueHandle<VkImageView>{imageView, device};
}

} // namespace blocks::render::vulkan
