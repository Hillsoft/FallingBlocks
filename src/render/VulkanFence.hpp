#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

class VulkanFence {
 public:
  VulkanFence(VulkanGraphicsDevice& device, bool preSignalled);
  ~VulkanFence();

  VulkanFence(const VulkanFence& other) = delete;
  VulkanFence& operator=(const VulkanFence& other) = delete;

  VulkanFence(VulkanFence&& other) noexcept;
  VulkanFence& operator=(VulkanFence&& other) noexcept;

  VkFence getRawFence() { return fence_; }

  void wait() const;
  void reset();
  void waitAndReset();

 private:
  VulkanGraphicsDevice* device_;
  VkFence fence_;
};

} // namespace blocks::render
