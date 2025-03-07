#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace tetris::render {

class VulkanImageView {
 public:
  VulkanImageView(
      VulkanGraphicsDevice& device, VkImage image, VkFormat imageFormat);
  ~VulkanImageView();

  VulkanImageView(const VulkanImageView& other) = delete;
  VulkanImageView& operator=(const VulkanImageView& other) = delete;

  VulkanImageView(VulkanImageView&& other) noexcept;
  VulkanImageView& operator=(VulkanImageView&& other) noexcept;

 private:
  VulkanGraphicsDevice* device_;
  VkImageView imageView_;
};

} // namespace tetris::render
