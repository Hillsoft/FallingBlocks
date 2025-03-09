#pragma once

#include "render/VulkanDeviceMemory.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"

namespace blocks::render {

class VulkanBuffer {
 public:
  VulkanBuffer(VulkanGraphicsDevice& device, std::span<char> data);

  VkBuffer getRawBuffer() { return rawBuffer_.getRawBuffer(); }

 private:
  VulkanRawBuffer rawBuffer_;
  VulkanDeviceMemory memory_;
};

} // namespace blocks::render
