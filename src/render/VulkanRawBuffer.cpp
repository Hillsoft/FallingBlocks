#include "render/VulkanRawBuffer.hpp"

#include <stdexcept>

namespace blocks::render {

VulkanRawBuffer::VulkanRawBuffer(
    VulkanGraphicsDevice& device, std::span<char> data)
    : device_(&device), buffer_(nullptr) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = data.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult result =
      vkCreateBuffer(device.getRawDevice(), &bufferInfo, nullptr, &buffer_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create buffer"};
  }
}

VulkanRawBuffer::~VulkanRawBuffer() {
  if (buffer_ != nullptr) {
    vkDestroyBuffer(device_->getRawDevice(), buffer_, nullptr);
  }
}

VulkanRawBuffer::VulkanRawBuffer(VulkanRawBuffer&& other) noexcept
    : device_(other.device_), buffer_(other.buffer_) {
  other.device_ = nullptr;
  other.buffer_ = nullptr;
}

VulkanRawBuffer& VulkanRawBuffer::operator=(VulkanRawBuffer&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(buffer_, other.buffer_);

  return *this;
}

} // namespace blocks::render
