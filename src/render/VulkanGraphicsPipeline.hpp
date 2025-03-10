#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPipelineLayout.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanUniqueHandle.hpp"

namespace blocks::render {

class VulkanGraphicsPipeline {
 public:
  VulkanGraphicsPipeline(
      VulkanGraphicsDevice& device,
      VkFormat imageFormat,
      VulkanVertexShader& vertexShader,
      VulkanShader& fragmentShader);

  VkPipeline getRawPipeline() { return pipeline_.get(); }
  VulkanRenderPass& getRenderPass() { return renderPass_; }

 private:
  VulkanPipelineLayout pipelineLayout_;
  VulkanRenderPass renderPass_;
  VulkanUniqueHandle<VkPipeline> pipeline_;
};

} // namespace blocks::render
