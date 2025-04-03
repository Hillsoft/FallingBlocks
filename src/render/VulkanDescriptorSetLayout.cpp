#include "render/VulkanDescriptorSetLayout.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
    VulkanGraphicsDevice& device)
    : descriptorSet_(nullptr, nullptr) {
  std::array<VkDescriptorSetLayoutBinding, 2> samplerLayoutBindings{};
  samplerLayoutBindings[0].binding = 0;
  samplerLayoutBindings[0].descriptorCount = 1;
  samplerLayoutBindings[0].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBindings[0].pImmutableSamplers = nullptr;
  samplerLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  samplerLayoutBindings[1].binding = 1;
  samplerLayoutBindings[1].descriptorCount = 1;
  samplerLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  samplerLayoutBindings[1].pImmutableSamplers = nullptr;
  samplerLayoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(samplerLayoutBindings.size());
  layoutInfo.pBindings = samplerLayoutBindings.data();

  VkDescriptorSetLayout layout;
  if (vkCreateDescriptorSetLayout(
          device.getRawDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create descriptor set"};
  }
  descriptorSet_ = vulkan::UniqueHandle<VkDescriptorSetLayout>(
      layout, device.getRawDevice());
}

} // namespace blocks::render
