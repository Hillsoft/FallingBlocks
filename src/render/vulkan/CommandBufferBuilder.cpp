#include "render/vulkan/CommandBufferBuilder.hpp"

#include <cstdint>
#include <span>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

CommandBufferBuilder::CommandBufferBuilder(
    VkCommandPool commandPool, VkCommandBufferLevel level)
    : allocInfo_() {
  allocInfo_.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo_.commandPool = commandPool;
  allocInfo_.level = level;
  allocInfo_.commandBufferCount = 1;
}

UniqueHandle<VkCommandBuffer> CommandBufferBuilder::build(
    VkDevice device) const {
  VkCommandBuffer commandBuffer;
  if (vkAllocateCommandBuffers(device, &allocInfo_, &commandBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to allocate command buffer"};
  }
  return UniqueHandle<VkCommandBuffer>(
      commandBuffer, {device, allocInfo_.commandPool});
}

void beginSingleTimeCommandBuffer(VkCommandBuffer buffer) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to begin recording command buffer"};
  }
}

void endSingleTimeCommandBuffer(
    VkCommandBuffer buffer,
    VkQueue submitQueue,
    std::span<const VkSemaphore> waitSemaphores,
    std::span<const VkSemaphore> signalSemaphores,
    VkFence signalFence) {
  if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to end recording command buffer"};
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &buffer;
  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.signalSemaphoreCount =
      static_cast<uint32_t>(signalSemaphores.size());
  submitInfo.pSignalSemaphores = signalSemaphores.data();

  if (vkQueueSubmit(submitQueue, 1, &submitInfo, signalFence) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to submit command buffer"};
  }
}

} // namespace blocks::render::vulkan
