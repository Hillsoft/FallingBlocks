#include "render/renderables/RenderableColor2D.hpp"

#include <cstdint>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/RenderableObject.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"
#include "render/shaders/Col2DShader.hpp"

namespace blocks::render {

RenderableObject RenderableColor2D::create(
    VulkanGraphicsDevice& device,
    ShaderProgramManager& programManager,
    TextureManager& textureManager,
    int maxFramesInFlight) {
  VulkanShaderProgram* shaderProgram =
      &programManager.getOrCreate<Col2DShader>();
  VulkanDescriptorPool descriptorPool{
      device, shaderProgram->getDescriptorSetLayout(), maxFramesInFlight};
  VulkanBuffer vertexAttributes = getQuadVertexAttributesBuffer(device);

  const auto& descriptorSets = descriptorPool.getDescriptorSets();
  std::vector<VkWriteDescriptorSet> descriptorWrites;
  descriptorWrites.reserve(descriptorSets.size());

  vkUpdateDescriptorSets(
      device.getRawDevice(),
      static_cast<uint32_t>(descriptorWrites.size()),
      descriptorWrites.data(),
      0,
      nullptr);

  return RenderableObject{
      shaderProgram,
      std::move(descriptorPool),
      std::move(vertexAttributes),
      sizeof(InstanceData)};
}

} // namespace blocks::render
