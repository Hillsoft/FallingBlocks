#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanImageView {
 public:
  VulkanImageView(
      VulkanGraphicsDevice& device, VkImage image, VkFormat imageFormat);

  VkImageView getRawImageView() { return imageView_.get(); }

 private:
  vulkan::UniqueHandle<VkImageView> imageView_;
};

} // namespace blocks::render
