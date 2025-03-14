#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanPipelineLayout {
 public:
  VulkanPipelineLayout(
      VulkanGraphicsDevice& device,
      VulkanDescriptorSetLayout& descriptorLayout);

  VkPipelineLayout getRawLayout() { return layout_.get(); }

 private:
  vulkan::UniqueHandle<VkPipelineLayout> layout_;
};

} // namespace blocks::render
