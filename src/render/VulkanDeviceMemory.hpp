#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"

namespace blocks::render {

class VulkanDeviceMemory {
 public:
  VulkanDeviceMemory(VulkanGraphicsDevice& device, VulkanRawBuffer& rawBuffer);
  ~VulkanDeviceMemory();

  VulkanDeviceMemory(const VulkanDeviceMemory& other) = delete;
  VulkanDeviceMemory& operator=(const VulkanDeviceMemory& other) = delete;

  VulkanDeviceMemory(VulkanDeviceMemory&& other) noexcept;
  VulkanDeviceMemory& operator=(VulkanDeviceMemory&& other) noexcept;

  VkDeviceMemory getRawMemory() { return memory_; }

 private:
  VulkanGraphicsDevice* device_;
  VkDeviceMemory memory_;
};

} // namespace blocks::render
