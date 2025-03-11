#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

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
  vulkan::UniqueHandle<VkFence> fence_;
};

} // namespace blocks::render
