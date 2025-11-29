#include "render/vulkan/DescriptorSetLayoutBuilder.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder() = default;

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    uint32_t descriptorCount,
    VkShaderStageFlags stageFlags) {
  bindings_.emplace_back(
      VkDescriptorSetLayoutBinding{
          .binding = binding,
          .descriptorType = descriptorType,
          .descriptorCount = descriptorCount,
          .stageFlags = stageFlags,
          .pImmutableSamplers = nullptr});
  return *this;
}

UniqueHandle<VkDescriptorSetLayout> DescriptorSetLayoutBuilder::build(
    VkDevice device) {
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings_.size());
  layoutInfo.pBindings = bindings_.data();

  VkDescriptorSetLayout layout = nullptr;
  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to create descriptor set"};
  }
  return vulkan::UniqueHandle<VkDescriptorSetLayout>{layout, device};
}

} // namespace blocks::render::vulkan
