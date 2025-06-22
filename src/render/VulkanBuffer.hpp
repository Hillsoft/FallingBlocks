#pragma once

#include <cstddef>
#include <span>
#include <GLFW/glfw3.h>
#include "render/VulkanDeviceMemory.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"

namespace blocks::render {

class VulkanBuffer {
 public:
  VulkanBuffer(
      VulkanGraphicsDevice& device,
      std::span<std::byte> data,
      VkBufferUsageFlags usageFlags);

  VkBuffer getRawBuffer() { return rawBuffer_.getRawBuffer(); }
  VkDeviceMemory getRawMemory() { return memory_.getRawMemory(); }

 private:
  VulkanRawBuffer rawBuffer_;
  VulkanDeviceMemory memory_;
};

} // namespace blocks::render
