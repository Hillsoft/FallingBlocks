#pragma once

#include <GLFW/glfw3.h>

#include <cstdint>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

uint32_t findMemoryType(
    const VkMemoryRequirements& requirements,
    const VkPhysicalDeviceMemoryProperties& memProperties,
    VkMemoryPropertyFlags properties);

class VulkanDeviceMemory {
 public:
  VulkanDeviceMemory(VulkanGraphicsDevice& device, VulkanRawBuffer& rawBuffer);

  VkDeviceMemory getRawMemory() { return memory_.get(); }

 private:
  vulkan::UniqueHandle<VkDeviceMemory> memory_;
};

} // namespace blocks::render
