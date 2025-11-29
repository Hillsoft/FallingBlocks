#include "render/renderables/RenderableTex2D.hpp"

#include <cstdint>
#include <filesystem>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "render/Quad.hpp"
#include "render/RenderableObject.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"
#include "render/VulkanTexture.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"
#include "render/shaders/Tex2DShader.hpp"

namespace blocks::render {

RenderableObject RenderableTex2D::create(
    const std::filesystem::path& texturePath,
    VulkanGraphicsDevice& device,
    ShaderProgramManager& programManager,
    TextureManager& textureManager,
    int maxFramesInFlight) {
  VulkanShaderProgram* shaderProgram =
      &programManager.getOrCreate<Tex2DShader>();
  VulkanDescriptorPool descriptorPool{
      device, shaderProgram->getDescriptorSetLayout(), maxFramesInFlight};
  VulkanBuffer vertexAttributes = getQuadVertexAttributesBuffer(device);
  VulkanTexture* texture = &textureManager.getOrCreate(texturePath);

  const auto& descriptorSets = descriptorPool.getDescriptorSets();
  std::vector<VkWriteDescriptorSet> descriptorWrites;
  descriptorWrites.reserve(descriptorSets.size());

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = texture->getImageView();
  imageInfo.sampler = texture->getSampler();

  for (const auto& set : descriptorSets) {
    auto& curWrite = descriptorWrites.emplace_back();
    curWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    curWrite.dstSet = set;
    curWrite.dstBinding = 0;
    curWrite.dstArrayElement = 0;
    curWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    curWrite.descriptorCount = 1;
    curWrite.pImageInfo = &imageInfo;
  }

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
