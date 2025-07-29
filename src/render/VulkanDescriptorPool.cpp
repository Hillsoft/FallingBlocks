#include "render/VulkanDescriptorPool.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanDescriptorPool::VulkanDescriptorPool(
    VulkanGraphicsDevice& device,
    VkDescriptorSetLayout layout,
    int maxFramesInFlight)
    : pool_(nullptr, nullptr) {
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

  poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

  VkDescriptorPool pool;
  if (vkCreateDescriptorPool(
          device.getRawDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create descriptor pool"};
  }

  pool_ = vulkan::UniqueHandle<VkDescriptorPool>(pool, device.getRawDevice());

  std::vector<VkDescriptorSetLayout> rawLayouts(maxFramesInFlight, layout);
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
