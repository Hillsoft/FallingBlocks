#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanRenderPass {
 public:
  VulkanRenderPass(VulkanGraphicsDevice& device, VkFormat imageFormat);

  VkRenderPass getRawRenderPass() { return renderPass_.get(); }

 private:
  vulkan::UniqueHandle<VkRenderPass> renderPass_;
};

} // namespace blocks::render
