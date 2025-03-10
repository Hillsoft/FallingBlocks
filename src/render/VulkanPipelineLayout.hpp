#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanPipelineLayout {
 public:
  VulkanPipelineLayout(VulkanGraphicsDevice& device);

  VkPipelineLayout getRawLayout() { return layout_.get(); }

 private:
  VulkanUniqueHandle<VkPipelineLayout> layout_;
};

} // namespace blocks::render
