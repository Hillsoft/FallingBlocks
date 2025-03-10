#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanFrameBuffer {
 public:
  VulkanFrameBuffer(
      VulkanGraphicsDevice& device,
      VulkanRenderPass& renderPass,
      VulkanImageView& imageView,
      VkExtent2D extent);

  VkFramebuffer getRawFrameBuffer() { return frameBuffer_.get(); }
  VkExtent2D getExtent() const { return extent_; }

 private:
  VkExtent2D extent_;
  VulkanUniqueHandle<VkFramebuffer> frameBuffer_;
};

} // namespace blocks::render
