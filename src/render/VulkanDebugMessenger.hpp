#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanDebugMessenger {
 public:
  VulkanDebugMessenger(VulkanInstance& instance);

 private:
  vulkan::UniqueHandle<VkDebugUtilsMessengerEXT> debugMessenger_;
};

} // namespace blocks::render
