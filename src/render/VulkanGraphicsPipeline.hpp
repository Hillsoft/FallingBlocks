#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanPipelineLayout.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/VulkanShader.hpp"

namespace blocks::render {

class VulkanGraphicsPipeline {
 public:
  VulkanGraphicsPipeline(
      VulkanGraphicsDevice& device,
      VkFormat imageFormat,
      VulkanShader& vertexShader,
      VulkanShader& fragmentShader);
  ~VulkanGraphicsPipeline();

  VulkanGraphicsPipeline(const VulkanGraphicsPipeline& other) = delete;
  VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline& other) =
      delete;

  VulkanGraphicsPipeline(VulkanGraphicsPipeline&& other) noexcept;
  VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&& other) noexcept;

  VkPipeline getRawPipeline() { return pipeline_; }
  VulkanRenderPass& getRenderPass() { return renderPass_; }

 private:
  VulkanGraphicsDevice* device_;
  VulkanPipelineLayout pipelineLayout_;
  VulkanRenderPass renderPass_;
  VkPipeline pipeline_;
};

} // namespace blocks::render
