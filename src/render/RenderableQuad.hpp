#pragma once

#include <cstdint>
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
      VulkanGraphicsDevice& device,
      VulkanCommandPool& commandPool,
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
