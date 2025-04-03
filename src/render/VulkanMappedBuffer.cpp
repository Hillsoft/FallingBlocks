#include "render/VulkanMappedBuffer.hpp"

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

VulkanMappedBuffer::VulkanMappedBuffer(
    VulkanGraphicsDevice& device, size_t size, VkBufferUsageFlags usageFlags)
    : rawBuffer_(device, size, usageFlags),
      memory_(device, rawBuffer_),
      mappedBuffer_(nullptr) {
  vkBindBufferMemory(
      device.getRawDevice(),
      rawBuffer_.getRawBuffer(),
      memory_.getRawMemory(),
      0);

  vkMapMemory(
      device.getRawDevice(),
      memory_.getRawMemory(),
      0,
      size,
      0,
      &mappedBuffer_);
}

} // namespace blocks::render
