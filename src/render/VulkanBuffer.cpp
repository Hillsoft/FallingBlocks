#include "render/VulkanBuffer.hpp"

#include <cstring>
#include <span>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

VulkanBuffer::VulkanBuffer(VulkanGraphicsDevice& device, std::span<char> data)
    : rawBuffer_(device, data), memory_(device, rawBuffer_) {
  vkBindBufferMemory(
      device.getRawDevice(),
      rawBuffer_.getRawBuffer(),
      memory_.getRawMemory(),
      0);

  void* buffData;
  vkMapMemory(
      device.getRawDevice(),
      memory_.getRawMemory(),
      0,
      data.size(),
      0,
      &buffData);
  memcpy(buffData, data.data(), data.size());
  vkUnmapMemory(device.getRawDevice(), memory_.getRawMemory());
}

} // namespace blocks::render
