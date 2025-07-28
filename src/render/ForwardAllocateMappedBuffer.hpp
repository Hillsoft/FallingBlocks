#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanMappedBuffer.hpp"

namespace blocks::render {

class ForwardAllocateMappedBuffer {
 public:
  struct Allocation {
    VkBuffer buffer;
    size_t bufferOffset;
    void* ptr;
  };

  ForwardAllocateMappedBuffer(
      VulkanGraphicsDevice& device, VkBufferUsageFlags usageFlags);

  Allocation alloc(size_t size);
  void reset();

 private:
  VulkanGraphicsDevice* device_;
  VkBufferUsageFlags usageFlags_;

  std::vector<VulkanMappedBuffer> buffers_;
  size_t curBuffer_;
  size_t bufferOffset_;
};

} // namespace blocks::render
