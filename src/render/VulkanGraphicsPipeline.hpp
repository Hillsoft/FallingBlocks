#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPipelineLayout.hpp"
#include "render/VulkanShader.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanGraphicsPipeline {
 public:
  VulkanGraphicsPipeline(
      VulkanGraphicsDevice& device,
      VkFormat imageFormat,
      VkRenderPass renderPass,
      VulkanVertexShader& vertexShader,
      VulkanShader& fragmentShader,
      VkDescriptorSetLayout descriptorLayout);

  VkPipeline getRawPipeline() { return pipeline_.get(); }
  VulkanPipelineLayout& getPipelineLayout() { return pipelineLayout_; }

 private:
  VulkanPipelineLayout pipelineLayout_;
  vulkan::UniqueHandle<VkPipeline> pipeline_;
};

} // namespace blocks::render
