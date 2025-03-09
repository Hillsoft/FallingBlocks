#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

class VulkanSemaphore {
 public:
  VulkanSemaphore(VulkanGraphicsDevice& device);
  ~VulkanSemaphore();

  VulkanSemaphore(const VulkanSemaphore& other) = delete;
  VulkanSemaphore& operator=(const VulkanSemaphore& other) = delete;

  VulkanSemaphore(VulkanSemaphore&& other) noexcept;
  VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept;

  VkSemaphore getRawSemaphore() { return semaphore_; }

 private:
  VulkanGraphicsDevice* device_;
  VkSemaphore semaphore_;
};

} // namespace blocks::render
