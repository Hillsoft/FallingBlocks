#include "render/shaders/Col2DShader.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
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
  descriptor.emplace_back(VkVertexInputAttributeDescription{
      .location = locationOffset,
      .binding = binding,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(Col2DShader::InstanceData, modelMatrix)});

  descriptor.emplace_back(VkVertexInputAttributeDescription{
      .location = locationOffset + 1,
      .binding = binding,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(Col2DShader::InstanceData, modelMatrix) +
          sizeof(float) * 4});

  descriptor.emplace_back(VkVertexInputAttributeDescription{
      .location = locationOffset + 2,
      .binding = binding,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(Col2DShader::InstanceData, modelMatrix) +
          sizeof(float) * 8});

  descriptor.emplace_back(VkVertexInputAttributeDescription{
      .location = locationOffset + 3,
      .binding = binding,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(Col2DShader::InstanceData, color)});
}

VulkanVertexShader getVertexShader(VulkanGraphicsDevice& device) {
  std::vector<VkVertexInputBindingDescription> bindings;
  bindings.reserve(2);

  bindings.emplace_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(UVVertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX});

  bindings.emplace_back(VkVertexInputBindingDescription{
      .binding = 1,
      .stride = sizeof(Col2DShader::InstanceData),
      .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE});

  std::vector<VkVertexInputAttributeDescription> attributes;
  attributes.reserve(4);

  uint32_t locationOffset = 0;
  UVVertex::appendVertexAttributeDescriptors(0, attributes, locationOffset);
  appendInstanceInputVertexAttributeDescriptors(1, attributes, locationOffset);

  return VulkanVertexShader{
      device,
      std::move(bindings),
      std::move(attributes),
      "shaders/colVertex.spv"};
}

} // namespace

VulkanShaderProgram Col2DShader::makeProgram(
    VulkanGraphicsDevice& device, VkRenderPass renderPass) {
  return {
      device,
      renderPass,
      getVertexShader(device),
      VulkanShader{device, "shaders/colFragment.spv"},
      vulkan::DescriptorSetLayoutBuilder().build(device.getRawDevice())};
}

} // namespace blocks::render
