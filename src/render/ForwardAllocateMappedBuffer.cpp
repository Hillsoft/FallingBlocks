#include "render/ForwardAllocateMappedBuffer.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

namespace {

constexpr size_t kChunkSize = 16ull * 1024;

}

ForwardAllocateMappedBuffer::ForwardAllocateMappedBuffer(
    VulkanGraphicsDevice& device, VkBufferUsageFlags usageFlags)
    : device_(&device), usageFlags_(usageFlags) {
  buffers_.emplace_back(*device_, kChunkSize, usageFlags_);
}

ForwardAllocateMappedBuffer::Allocation ForwardAllocateMappedBuffer::alloc(
    size_t size) {
  if (size > kChunkSize) {
    throw std::runtime_error{"Exceeded maximum allocation size"};
  }

  if (kChunkSize - bufferOffset_ < size) {
    bufferOffset_ = 0;
    curBuffer_++;
    if (curBuffer_ == buffers_.size()) {
      buffers_.emplace_back(*device_, kChunkSize, usageFlags_);
    }
  }

  Allocation result = {
      .buffer = buffers_[curBuffer_].getRawBuffer(),
      .bufferOffset = bufferOffset_,
      .ptr =
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)
      reinterpret_cast<std::byte*>(buffers_[curBuffer_].getMappedBuffer()) +
          bufferOffset_};

  bufferOffset_ += size;

  return result;
}

void ForwardAllocateMappedBuffer::reset() {
  curBuffer_ = 0;
  bufferOffset_ = 0;
}

} // namespace blocks::render
