#include "render/VulkanCommandBuffer.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/CommandBufferBuilder.hpp"

namespace blocks::render {

VulkanCommandBuffer::VulkanCommandBuffer(
    VulkanGraphicsDevice& device, VulkanCommandPool& commandPool)
    : commandBuffer_(
          vulkan::CommandBufferBuilder{
              commandPool.getRawCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY}
              .build(device.getRawDevice())),
      queue_(commandPool.getQueue()) {}

void VulkanCommandBuffer::submit(
    const std::vector<VkSemaphore>& waitSemaphores,
    const std::vector<VkSemaphore>& signalSemaphores,
    VkFence signalFence) {
  std::vector<VkPipelineStageFlags> waitStages;
  waitStages.reserve(waitSemaphores.size());
  for (size_t i = 0; i < waitSemaphores.size(); i++) {
    waitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.pWaitDstStageMask = waitStages.data();

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer_.get();

  submitInfo.signalSemaphoreCount =
      static_cast<uint32_t>(signalSemaphores.size());
  submitInfo.pSignalSemaphores = signalSemaphores.data();

  if (vkQueueSubmit(queue_, 1, &submitInfo, signalFence) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to submit draw command buffer"};
  }
}

} // namespace blocks::render
