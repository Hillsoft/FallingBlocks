#include "render/VulkanRawBuffer.hpp"

#include <stdexcept>

namespace blocks::render {

VulkanRawBuffer::VulkanRawBuffer(
    VulkanGraphicsDevice& device, std::span<char> data)
    : buffer_(nullptr, nullptr) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = data.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer buffer;
  VkResult result =
      vkCreateBuffer(device.getRawDevice(), &bufferInfo, nullptr, &buffer);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create buffer"};
  }

  buffer_ = VulkanUniqueHandle<VkBuffer>{buffer, device.getRawDevice()};
}

} // namespace blocks::render
