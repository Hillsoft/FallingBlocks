#include "render/RenderableQuad.hpp"

#include <cstdint>
#include <filesystem>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

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
      texture_(device, commandPool, texturePath) {
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

  vkUpdateDescriptorSets(
      device.getRawDevice(),
      static_cast<uint32_t>(descriptorWrites.size()),
      descriptorWrites.data(),
      0,
      nullptr);
}

} // namespace blocks::render
