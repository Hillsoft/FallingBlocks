#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanInstance {
 public:
  VulkanInstance();

  VkInstance getRawInstance() { return instance_.get(); }

 private:
  VulkanUniqueHandle<VkInstance> instance_;
};

} // namespace blocks::render
