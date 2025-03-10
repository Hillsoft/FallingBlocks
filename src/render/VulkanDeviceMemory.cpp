#include "render/VulkanDeviceMemory.hpp"

#include <stdexcept>

namespace blocks::render {

namespace {

uint32_t findMemoryType(
    const VkMemoryRequirements& requirements,
    const VkPhysicalDeviceMemoryProperties& memProperties,
    VkMemoryPropertyFlags properties) {
  uint32_t typeFilter = requirements.memoryTypeBits;

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (typeFilter & (1 << i) &&
        (memProperties.memoryTypes[i].propertyFlags &
         properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error{"No suitable memory found"};
}

} // namespace

VulkanDeviceMemory::VulkanDeviceMemory(
    VulkanGraphicsDevice& device, VulkanRawBuffer& rawBuffer)
    : memory_(nullptr, nullptr) {
  VkMemoryRequirements memRequirements{};
  vkGetBufferMemoryRequirements(
      device.getRawDevice(), rawBuffer.getRawBuffer(), &memRequirements);

  VkPhysicalDeviceMemoryProperties memProperties{};
  vkGetPhysicalDeviceMemoryProperties(
      device.physicalInfo().device, &memProperties);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
      memRequirements,
      memProperties,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkDeviceMemory memory;
  VkResult result =
      vkAllocateMemory(device.getRawDevice(), &allocInfo, nullptr, &memory);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to allocate memory"};
  }

  memory_ = VulkanUniqueHandle<VkDeviceMemory>{memory, device.getRawDevice()};
}

} // namespace blocks::render
