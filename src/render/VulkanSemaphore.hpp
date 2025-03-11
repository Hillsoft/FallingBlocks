#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanSemaphore {
 public:
  VulkanSemaphore(VulkanGraphicsDevice& device);

  VkSemaphore getRawSemaphore() { return semaphore_.get(); }

 private:
  vulkan::UniqueHandle<VkSemaphore> semaphore_;
};

} // namespace blocks::render
