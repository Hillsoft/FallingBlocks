#pragma once

#include <span>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class CommandBufferBuilder {
 public:
  explicit CommandBufferBuilder(
      VkCommandPool commandPool, VkCommandBufferLevel level);

  UniqueHandle<VkCommandBuffer> build(VkDevice device) const;

 private:
  VkCommandBufferAllocateInfo allocInfo_;
};

void beginSingleTimeCommandBuffer(VkCommandBuffer buffer);
void endSingleTimeCommandBuffer(
    VkCommandBuffer buffer,
    VkQueue submitQueue,
    std::span<const VkSemaphore> waitSemaphores,
    std::span<const VkSemaphore> signalSemaphores,
    VkFence signalFence);

} // namespace blocks::render::vulkan
