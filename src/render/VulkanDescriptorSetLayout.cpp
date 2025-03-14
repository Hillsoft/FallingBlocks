#include "render/VulkanDescriptorSetLayout.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
    VulkanGraphicsDevice& device)
    : descriptorSet_(nullptr, nullptr) {
  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 0;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &samplerLayoutBinding;

  VkDescriptorSetLayout layout;
  if (vkCreateDescriptorSetLayout(
          device.getRawDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create descriptor set"};
  }
  descriptorSet_ = vulkan::UniqueHandle<VkDescriptorSetLayout>(
      layout, device.getRawDevice());
}

} // namespace blocks::render
