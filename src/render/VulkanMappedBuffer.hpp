#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanDeviceMemory.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"

namespace blocks::render {

class VulkanMappedBuffer {
 public:
  VulkanMappedBuffer(
      VulkanGraphicsDevice& device, size_t size, VkBufferUsageFlags usageFlags);

  VkBuffer getRawBuffer() { return rawBuffer_.getRawBuffer(); }
  VkDeviceMemory getRawMemory() { return memory_.getRawMemory(); }
  void* getMappedBuffer() { return mappedBuffer_; }

 private:
  VulkanRawBuffer rawBuffer_;
  VulkanDeviceMemory memory_;
  void* mappedBuffer_;
};

} // namespace blocks::render
