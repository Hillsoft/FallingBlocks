#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class ImageViewBuilder {
 public:
  explicit ImageViewBuilder(VkImage image, VkFormat imageFormat);

  ImageViewBuilder& setMipLevels(uint32_t mipLevels);

  UniqueHandle<VkImageView> build(VkDevice device);

 private:
  VkImageViewCreateInfo createInfo_;
};

} // namespace blocks::render::vulkan
