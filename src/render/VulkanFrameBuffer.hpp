#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanRenderPass.hpp"

namespace blocks::render {

class VulkanFrameBuffer {
 public:
  VulkanFrameBuffer(
      VulkanGraphicsDevice& device,
      VulkanRenderPass& renderPass,
      VulkanImageView& imageView,
      VkExtent2D extent);
  ~VulkanFrameBuffer();

  VulkanFrameBuffer(const VulkanFrameBuffer& other) = delete;
  VulkanFrameBuffer& operator=(const VulkanFrameBuffer& other) = delete;

  VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept;
  VulkanFrameBuffer& operator=(VulkanFrameBuffer&& other) noexcept;

  VkFramebuffer getRawFrameBuffer() { return frameBuffer_; }
  VkExtent2D getExtent() const { return extent_; }

 private:
  VulkanGraphicsDevice* device_;
  VkExtent2D extent_;
  VkFramebuffer frameBuffer_;
};

} // namespace blocks::render
