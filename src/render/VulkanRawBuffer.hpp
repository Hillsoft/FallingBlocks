#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanRawBuffer {
 public:
  VulkanRawBuffer(
      VulkanGraphicsDevice& device, size_t size, VkBufferUsageFlags usageFlags);

  VkBuffer getRawBuffer() { return buffer_.get(); }

 private:
  vulkan::UniqueHandle<VkBuffer> buffer_;
};

} // namespace blocks::render
