#include "render/VulkanCommandBuffer.hpp"

#include <array>
#include <stdexcept>

#include "util/debug.hpp"

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
    const std::vector<VulkanSemaphore*>& waitSemaphores,
    const std::vector<VulkanSemaphore*>& signalSemaphores,
    VulkanFence* signalFence) {
  std::vector<VkSemaphore> waitSemaphoresRaw;
  waitSemaphoresRaw.reserve(waitSemaphores.size());
  std::vector<VkPipelineStageFlags> waitStages;
  waitStages.reserve(waitSemaphores.size());
  for (const auto& semaphore : waitSemaphores) {
    DEBUG_ASSERT(semaphore != nullptr);
    waitSemaphoresRaw.emplace_back(semaphore->getRawSemaphore());
    waitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
  }

  std::vector<VkSemaphore> signalSemaphoresRaw;
  signalSemaphoresRaw.reserve(signalSemaphores.size());
  for (const auto& semaphore : signalSemaphores) {
    DEBUG_ASSERT(semaphore != nullptr);
    signalSemaphoresRaw.emplace_back(semaphore->getRawSemaphore());
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  submitInfo.waitSemaphoreCount =
      static_cast<uint32_t>(waitSemaphoresRaw.size());
  submitInfo.pWaitSemaphores = waitSemaphoresRaw.data();
  submitInfo.pWaitDstStageMask = waitStages.data();

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer_;

  submitInfo.signalSemaphoreCount =
      static_cast<uint32_t>(signalSemaphoresRaw.size());
  submitInfo.pSignalSemaphores = signalSemaphoresRaw.data();

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
