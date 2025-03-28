#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPipelineLayout.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanShader.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanGraphicsPipeline {
 public:
  VulkanGraphicsPipeline(
      VulkanGraphicsDevice& device,
      VkFormat imageFormat,
      VulkanVertexShader& vertexShader,
      VulkanShader& fragmentShader,
      VulkanDescriptorSetLayout& descriptorLayout);

  VkPipeline getRawPipeline() { return pipeline_.get(); }
  VulkanPipelineLayout& getPipelineLayout() { return pipelineLayout_; }
  VulkanRenderPass& getRenderPass() { return renderPass_; }

 private:
  VulkanPipelineLayout pipelineLayout_;
  VulkanRenderPass renderPass_;
  vulkan::UniqueHandle<VkPipeline> pipeline_;
};

} // namespace blocks::render
