#include "render/VulkanDescriptorPool.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanDescriptorPool::VulkanDescriptorPool(
    VulkanGraphicsDevice& device,
    VulkanDescriptorSetLayout& layout,
    int maxFramesInFlight)
    : pool_(nullptr, nullptr) {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSize.descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

  VkDescriptorPool pool;
  if (vkCreateDescriptorPool(
          device.getRawDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create descriptor pool"};
  }

  pool_ = vulkan::UniqueHandle<VkDescriptorPool>(pool, device.getRawDevice());

  std::vector<VkDescriptorSetLayout> rawLayouts(
      maxFramesInFlight, layout.getRawLayout());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = maxFramesInFlight;
  allocInfo.pSetLayouts = rawLayouts.data();

  descriptorSets_.resize(maxFramesInFlight);
  if (vkAllocateDescriptorSets(
          device.getRawDevice(), &allocInfo, descriptorSets_.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to create descriptor sets"};
  }
}

} // namespace blocks::render
