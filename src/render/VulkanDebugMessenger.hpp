#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanDebugMessenger {
 public:
  VulkanDebugMessenger(VulkanInstance& instance);

 private:
  VulkanUniqueHandle<VkDebugUtilsMessengerEXT> debugMessenger_;
};

} // namespace blocks::render
