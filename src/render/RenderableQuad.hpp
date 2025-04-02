#pragma once

#include <cstdint>
#include <filesystem>
#include <GLFW/glfw3.h>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanTexture.hpp"

namespace blocks::render {

class RenderSubSystem;

class RenderableQuad {
 public:
  RenderableQuad(
      const std::filesystem::path& texturePath,
      VulkanGraphicsDevice& device,
      VulkanCommandPool& commandPool,
      VkRenderPass renderPass,
      uint32_t maxFramesInFlight);

 private:
  VulkanVertexShader vertexShader_;
  VulkanShader fragmentShader_;
  VulkanDescriptorSetLayout descriptorSetLayout_;
  VulkanDescriptorPool descriptorPool_;
  VulkanGraphicsPipeline pipeline_;
  VulkanBuffer vertexAttributes_;
  VulkanTexture texture_;

 public:
  friend class RenderSubSystem;
};

} // namespace blocks::render
