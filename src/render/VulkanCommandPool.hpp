#pragma once

#include <vulkan/vulkan_core.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanCommandPool {
 public:
  VulkanCommandPool(VulkanGraphicsDevice& device, bool loadingQueue);

  VkCommandPool getRawCommandPool() { return commandPool_.get(); }
  VkQueue getQueue() { return queue_; }

 private:
  VkQueue queue_;
  vulkan::UniqueHandle<VkCommandPool> commandPool_;
};

} // namespace blocks::render
