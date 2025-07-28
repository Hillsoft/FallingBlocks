#include "render/ForwardAllocateMappedBuffer.hpp"

#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

namespace {

constexpr size_t kChunkSize = 16 * 1024;

}

ForwardAllocateMappedBuffer::ForwardAllocateMappedBuffer(
    VulkanGraphicsDevice& device, VkBufferUsageFlags usageFlags)
    : device_(&device),
      usageFlags_(usageFlags),
      curBuffer_(0),
      bufferOffset_(0) {
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
      .ptr = reinterpret_cast<void*>(
          reinterpret_cast<size_t>(buffers_[curBuffer_].getMappedBuffer()) +
          bufferOffset_)};

  bufferOffset_ += size;

  return result;
}

void ForwardAllocateMappedBuffer::reset() {
  curBuffer_ = 0;
  bufferOffset_ = 0;
}

} // namespace blocks::render
