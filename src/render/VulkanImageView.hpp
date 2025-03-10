#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanImageView {
 public:
  VulkanImageView(
      VulkanGraphicsDevice& device, VkImage image, VkFormat imageFormat);

  VkImageView getRawImageView() { return imageView_.get(); }

 private:
  VulkanUniqueHandle<VkImageView> imageView_;
};

} // namespace blocks::render
