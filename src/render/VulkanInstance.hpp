#pragma once

#include <vulkan/vulkan_core.h>

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
