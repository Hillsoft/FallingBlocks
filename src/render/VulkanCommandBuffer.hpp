#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanCommandBuffer {
 public:
  VulkanCommandBuffer(
      VulkanGraphicsDevice& device, VulkanCommandPool& commandPool);

  void submit(
      const std::vector<VkSemaphore>& waitSemaphores,
      const std::vector<VkSemaphore>& signalSemaphores,
      VkFence signalFence);

  VkCommandBuffer getRawBuffer() { return commandBuffer_.get(); }

 private:
  vulkan::UniqueHandle<VkCommandBuffer> commandBuffer_;
  VkQueue queue_;
};

} // namespace blocks::render
