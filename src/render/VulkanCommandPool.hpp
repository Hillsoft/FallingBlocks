#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanCommandPool {
 public:
  VulkanCommandPool(VulkanGraphicsDevice& device);

  VkCommandPool getRawCommandPool() { return commandPool_.get(); }
  VkQueue getQueue() { return queue_; }

 private:
  VkQueue queue_;
  VulkanUniqueHandle<VkCommandPool> commandPool_;
};

} // namespace blocks::render
