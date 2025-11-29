#include "render/VulkanDeviceMemory.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanRawBuffer.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

uint32_t findMemoryType(
    const VkMemoryRequirements& requirements,
    const VkPhysicalDeviceMemoryProperties& memProperties,
    VkMemoryPropertyFlags properties) {
  const uint32_t typeFilter = requirements.memoryTypeBits;

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1ull << i)) != 0 &&
        (memProperties.memoryTypes[i].propertyFlags &
         properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error{"No suitable memory found"};
}

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
      // NOLINTNEXTLINE(hicpp-signed-bitwise)
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkDeviceMemory memory = nullptr;
  const VkResult result =
      vkAllocateMemory(device.getRawDevice(), &allocInfo, nullptr, &memory);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to allocate memory"};
  }

  memory_ = vulkan::UniqueHandle<VkDeviceMemory>{memory, device.getRawDevice()};
}

} // namespace blocks::render
