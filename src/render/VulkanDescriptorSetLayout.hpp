#pragma once

#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanDescriptorSetLayout {
 public:
  VulkanDescriptorSetLayout(VulkanGraphicsDevice& device);

  VkDescriptorSetLayout getRawLayout() { return descriptorSet_.get(); }

 private:
  vulkan::UniqueHandle<VkDescriptorSetLayout> descriptorSet_;
};

} // namespace blocks::render
