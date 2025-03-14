#include "render/VulkanRawBuffer.hpp"

#include <span>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanRawBuffer::VulkanRawBuffer(
    VulkanGraphicsDevice& device,
    std::span<char> data,
    VkBufferUsageFlags usageFlags)
    : buffer_(nullptr, nullptr) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = data.size();
  bufferInfo.usage = usageFlags;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer buffer;
  VkResult result =
      vkCreateBuffer(device.getRawDevice(), &bufferInfo, nullptr, &buffer);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create buffer"};
  }

  buffer_ = vulkan::UniqueHandle<VkBuffer>{buffer, device.getRawDevice()};
}

} // namespace blocks::render
