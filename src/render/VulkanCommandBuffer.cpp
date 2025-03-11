#include "render/VulkanCommandBuffer.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanCommandPool.hpp"
#include "render/VulkanFence.hpp"
#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

VulkanCommandBuffer::VulkanCommandBuffer(
    VulkanGraphicsDevice& device, VulkanCommandPool& commandPool)
    : commandBuffer_(nullptr), queue_(commandPool.getQueue()) {
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

void VulkanCommandBuffer::submit(
    const std::vector<VkSemaphore>& waitSemaphores,
    const std::vector<VkSemaphore>& signalSemaphores,
    VulkanFence* signalFence) {
  std::vector<VkPipelineStageFlags> waitStages;
  waitStages.reserve(waitSemaphores.size());
  for (const auto& semaphore : waitSemaphores) {
    waitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.pWaitDstStageMask = waitStages.data();

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer_;

  submitInfo.signalSemaphoreCount =
      static_cast<uint32_t>(signalSemaphores.size());
  submitInfo.pSignalSemaphores = signalSemaphores.data();

  if (vkQueueSubmit(
          queue_,
          1,
          &submitInfo,
          signalFence != nullptr ? signalFence->getRawFence() : nullptr) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to submit draw command buffer"};
  }
}

} // namespace blocks::render
