#pragma once

#include <span>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

class VulkanRawBuffer {
 public:
  VulkanRawBuffer(VulkanGraphicsDevice& device, std::span<char> data);
  ~VulkanRawBuffer();

  VulkanRawBuffer(const VulkanRawBuffer& other) = delete;
  VulkanRawBuffer& operator=(const VulkanRawBuffer& other) = delete;

  VulkanRawBuffer(VulkanRawBuffer&& other) noexcept;
  VulkanRawBuffer& operator=(VulkanRawBuffer&& other) noexcept;

  VkBuffer getRawBuffer() { return buffer_; }

 private:
  VulkanGraphicsDevice* device_;
  VkBuffer buffer_;
};

} // namespace blocks::render
