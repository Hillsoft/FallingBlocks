#include "render/VulkanCommandBuffer.hpp"

#include <stdexcept>

namespace tetris::render {

VulkanCommandBuffer::VulkanCommandBuffer(
    VulkanGraphicsDevice& device, VulkanCommandPool& commandPool)
    : commandBuffer_(nullptr) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool.getRawCommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkResult result = vkAllocateCommandBuffers(
      device.getRawDevice(), &allocInfo, &commandBuffer_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create command buffer"};
  }
}

} // namespace tetris::render
