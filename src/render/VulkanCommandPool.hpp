#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace tetris::render {

class VulkanCommandPool {
 public:
  VulkanCommandPool(VulkanGraphicsDevice& device);
  ~VulkanCommandPool();

  VulkanCommandPool(const VulkanCommandPool& other) = delete;
  VulkanCommandPool& operator=(const VulkanCommandPool& other) = delete;

  VulkanCommandPool(VulkanCommandPool&& other) noexcept;
  VulkanCommandPool& operator=(VulkanCommandPool&& other) noexcept;

  VkCommandPool getRawCommandPool() { return commandPool_; }
  VkQueue getQueue() { return queue_; }

 private:
  VulkanGraphicsDevice* device_;
  VkQueue queue_;
  VkCommandPool commandPool_;
};

} // namespace tetris::render
