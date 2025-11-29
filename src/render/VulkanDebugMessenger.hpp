#pragma once

#include <vulkan/vulkan_core.h>

#include "render/VulkanInstance.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanDebugMessenger {
 public:
  explicit VulkanDebugMessenger(VulkanInstance& instance);

 private:
  vulkan::UniqueHandle<VkDebugUtilsMessengerEXT> debugMessenger_;
};

} // namespace blocks::render
