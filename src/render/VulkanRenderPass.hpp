#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanRenderPass {
 public:
  VulkanRenderPass(VulkanGraphicsDevice& device, VkFormat imageFormat);

  VkRenderPass getRawRenderPass() { return renderPass_.get(); }

 private:
  VulkanUniqueHandle<VkRenderPass> renderPass_;
};

} // namespace blocks::render
