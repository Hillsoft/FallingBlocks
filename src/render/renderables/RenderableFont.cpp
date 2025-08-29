#include "render/renderables/RenderableFont.hpp"

#include <cstdint>
#include <memory>
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
#include "render/shaders/FontShader.hpp"

namespace blocks::render {

namespace {

class ExtraFontResources : public RenderableObject::ResourceHolder {
 public:
  ExtraFontResources(VulkanBuffer fontBuffer)
      : fontBuffer_(std::move(fontBuffer)) {}

 private:
  VulkanBuffer fontBuffer_;
};

} // namespace

RenderableObject RenderableFont::create(
    VulkanBuffer fontBuffer,
    VulkanGraphicsDevice& device,
    ShaderProgramManager& programManager,
    TextureManager& textureManager,
    int maxFramesInFlight) {
  VulkanShaderProgram* shaderProgram =
      &programManager.getOrCreate<FontShader>();
  VulkanDescriptorPool descriptorPool{
      device, shaderProgram->getDescriptorSetLayout(), maxFramesInFlight};
  VulkanBuffer vertexAttributes = getQuadVertexAttributesBuffer(device);

  const auto& descriptorSets = descriptorPool.getDescriptorSets();
  std::vector<VkWriteDescriptorSet> descriptorWrites;
  descriptorWrites.reserve(descriptorSets.size());

  VkDescriptorBufferInfo bufferInfo{
      .buffer = fontBuffer.getRawBuffer(),
      .offset = 0,
      .range = fontBuffer.size()};

  for (const auto& set : descriptorSets) {
    auto& curWrite = descriptorWrites.emplace_back();

    curWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    curWrite.dstSet = set;
    curWrite.dstBinding = 0;
    curWrite.dstArrayElement = 0;
    curWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    curWrite.descriptorCount = 1;
    curWrite.pBufferInfo = &bufferInfo;
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
      sizeof(InstanceData),
      std::make_unique<ExtraFontResources>(std::move(fontBuffer))};
}

} // namespace blocks::render
