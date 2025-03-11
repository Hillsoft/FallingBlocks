#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanDeviceMemory {
 public:
  VulkanDeviceMemory(VulkanGraphicsDevice& device, VulkanRawBuffer& rawBuffer);

  VkDeviceMemory getRawMemory() { return memory_.get(); }

 private:
  vulkan::UniqueHandle<VkDeviceMemory> memory_;
};

} // namespace blocks::render
