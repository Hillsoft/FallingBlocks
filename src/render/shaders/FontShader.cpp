#include "render/shaders/FontShader.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanShaderProgram.hpp"
#include "render/shaders/UVVertex.hpp"
#include "render/vulkan/DescriptorSetLayoutBuilder.hpp"

namespace blocks::render {

namespace {

void appendInstanceInputVertexAttributeDescriptors(
    uint32_t binding,
    std::vector<VkVertexInputAttributeDescription>& descriptor,
    uint32_t& locationOffset) {
  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset,
          .binding = binding,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(FontShader::InstanceData, modelMatrix)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 1,
          .binding = binding,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(FontShader::InstanceData, modelMatrix) +
              (sizeof(float) * 4)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 2,
          .binding = binding,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(FontShader::InstanceData, modelMatrix) +
              (sizeof(float) * 8)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 3,
          .binding = binding,
          .format = VK_FORMAT_R32_SINT,
          .offset = offsetof(FontShader::InstanceData, glyphStart)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 4,
          .binding = binding,
          .format = VK_FORMAT_R32_SINT,
          .offset = offsetof(FontShader::InstanceData, glyphEnd)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 5,
          .binding = binding,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(FontShader::InstanceData, uvToGlyphSpace)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 6,
          .binding = binding,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(FontShader::InstanceData, uvToGlyphSpace) +
              (sizeof(float) * 4)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 7,
          .binding = binding,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(FontShader::InstanceData, uvToGlyphSpace) +
              (sizeof(float) * 8)});
}

VulkanVertexShader getVertexShader(VulkanGraphicsDevice& device) {
  std::vector<VkVertexInputBindingDescription> bindings;
  bindings.reserve(2);

  bindings.emplace_back(
      VkVertexInputBindingDescription{
          .binding = 0,
          .stride = sizeof(UVVertex),
          .inputRate = VK_VERTEX_INPUT_RATE_VERTEX});

  bindings.emplace_back(
      VkVertexInputBindingDescription{
          .binding = 1,
          .stride = sizeof(FontShader::InstanceData),
          .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE});

  std::vector<VkVertexInputAttributeDescription> attributes;
  attributes.reserve(9);

  uint32_t locationOffset = 0;
  UVVertex::appendVertexAttributeDescriptors(0, attributes, locationOffset);
  appendInstanceInputVertexAttributeDescriptors(1, attributes, locationOffset);

  return VulkanVertexShader{
      device,
      std::move(bindings),
      std::move(attributes),
      "shaders/fontVertex.spv"};
}

} // namespace

VulkanShaderProgram FontShader::makeProgram(
    VulkanGraphicsDevice& device, VkRenderPass renderPass) {
  return {
      device,
      renderPass,
      getVertexShader(device),
      VulkanShader{device, "shaders/fontFragment.spv"},
      vulkan::DescriptorSetLayoutBuilder()
          .addBinding(
              0,
              VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
              1,
              VK_SHADER_STAGE_FRAGMENT_BIT)
          .build(device.getRawDevice())};
}

} // namespace blocks::render
