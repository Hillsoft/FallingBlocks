#pragma once

#include <GLFW/glfw3.h>

#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanInstance {
 public:
  VulkanInstance();

  VkInstance getRawInstance() { return instance_.get(); }

 private:
  vulkan::UniqueHandle<VkInstance> instance_;
};

} // namespace blocks::render
