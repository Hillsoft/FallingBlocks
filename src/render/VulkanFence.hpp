#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanFence {
 public:
  VulkanFence(VulkanGraphicsDevice& device, bool preSignalled);

  VkFence getRawFence() { return fence_.get(); }

  void wait() const;
  void reset();
  void waitAndReset();

 private:
  VulkanGraphicsDevice* device_;
  VulkanUniqueHandle<VkFence> fence_;
};

} // namespace blocks::render
