#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>
#include <GLFW/glfw3.h>
#include "math/vec.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanMappedBuffer.hpp"
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

  void setPosition(math::Vec<float, 2> pos0, math::Vec<float, 2> pos1);
  void bindDynamicDescriptors(VkDevice device, uint32_t currentFrame);

 private:
  VulkanVertexShader vertexShader_;
  VulkanShader fragmentShader_;
  VulkanDescriptorSetLayout descriptorSetLayout_;
  VulkanDescriptorPool descriptorPool_;
  VulkanGraphicsPipeline pipeline_;
  VulkanBuffer vertexAttributes_;
  std::vector<VulkanMappedBuffer> uniformBuffers_;
  VulkanTexture texture_;

  math::Vec<float, 2> pos0_;
  math::Vec<float, 2> pos1_;

 public:
  friend class RenderSubSystem;
};

} // namespace blocks::render
