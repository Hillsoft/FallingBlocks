#include "render/RenderableQuad.hpp"

#include <cstdint>
#include <filesystem>
#include <vector>
#include <GLFW/glfw3.h>
#include "math/vec.hpp"
#include "render/Quad.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanMappedBuffer.hpp"

namespace blocks::render {

namespace {

struct UniformData {
  math::Vec<float, 2> pos0;
  math::Vec<float, 2> pos1;
};

std::vector<VulkanMappedBuffer> makeUniformBuffers(
    VulkanGraphicsDevice& device, uint32_t maxFramesInFlight) {
  std::vector<VulkanMappedBuffer> result;
  result.reserve(maxFramesInFlight);

  for (size_t i = 0; i < maxFramesInFlight; i++) {
    result.emplace_back(
        device, sizeof(UniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  }

  return result;
}

} // namespace

RenderableQuad::RenderableQuad(
    const std::filesystem::path& texturePath,
    VulkanGraphicsDevice& device,
    VulkanCommandPool& commandPool,
    VkRenderPass renderPass,
    uint32_t maxFramesInFlight)
    : vertexShader_(getQuadVertexShader(device)),
      fragmentShader_(device, "shaders/fragment.spv"),
      descriptorSetLayout_(device),
      descriptorPool_(device, descriptorSetLayout_, maxFramesInFlight),
      pipeline_(
          device,
          VK_FORMAT_B8G8R8A8_SRGB,
          renderPass,
          vertexShader_,
          fragmentShader_,
          descriptorSetLayout_),
      vertexAttributes_(getQuadVertexAttributesBuffer(device)),
      uniformBuffers_(makeUniformBuffers(device, maxFramesInFlight)),
      texture_(device, commandPool, texturePath),
      pos0_(-1.0f, -1.0f),
      pos1_(1.0f, 1.0f) {
  const auto& descriptorSets = descriptorPool_.getDescriptorSets();
  std::vector<VkWriteDescriptorSet> descriptorWrites;
  descriptorWrites.reserve(descriptorSets.size());

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = texture_.getImageView().getRawImageView();
  imageInfo.sampler = texture_.getSampler();

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

  std::vector<VkDescriptorBufferInfo> bufferInfos;
  bufferInfos.reserve(maxFramesInFlight);

  for (size_t i = 0; i < maxFramesInFlight; i++) {
    auto& bufferInfo = bufferInfos.emplace_back();
    bufferInfo.buffer = uniformBuffers_[i].getRawBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformData);

    auto& curWrite = descriptorWrites.emplace_back();
    curWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    curWrite.dstSet = descriptorPool_.getDescriptorSets()[i];
    curWrite.dstBinding = 1;
    curWrite.dstArrayElement = 0;
    curWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    curWrite.descriptorCount = 1;
    curWrite.pBufferInfo = &bufferInfo;
  }

  vkUpdateDescriptorSets(
      device.getRawDevice(),
      static_cast<uint32_t>(descriptorWrites.size()),
      descriptorWrites.data(),
      0,
      nullptr);
}

void RenderableQuad::setPosition(
    math::Vec<float, 2> pos0, math::Vec<float, 2> pos1) {
  pos0_ = pos0;
  pos1_ = pos1;
}

void RenderableQuad::updateDynamicUniforms(
    VkDevice device, uint32_t currentFrame) {
  UniformData* uniformData = reinterpret_cast<UniformData*>(
      uniformBuffers_[currentFrame].getMappedBuffer());

  uniformData->pos0 = pos0_;
  uniformData->pos1 = pos1_;
}

} // namespace blocks::render
