#pragma once

#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class ImageViewBuilder {
 public:
  explicit ImageViewBuilder(VkImage image, VkFormat imageFormat);

  UniqueHandle<VkImageView> build(VkDevice device);

 private:
  VkImageViewCreateInfo createInfo_;
};

} // namespace blocks::render::vulkan
