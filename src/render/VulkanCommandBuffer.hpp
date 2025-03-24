#pragma once

#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanCommandPool.hpp"
#include "render/VulkanFrameBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanRenderPass.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanCommandBuffer {
 public:
  VulkanCommandBuffer(
      VulkanGraphicsDevice& device, VulkanCommandPool& commandPool);

  template <typename Fn>
  void runRenderPass(
      VulkanGraphicsPipeline& pipeline,
      VulkanFrameBuffer& frameBuffer,
      Fn&& fn);

  void submit(
      const std::vector<VkSemaphore>& waitSemaphores,
      const std::vector<VkSemaphore>& signalSemaphores,
      VkFence signalFence);

  VkCommandBuffer getRawBuffer() { return commandBuffer_.get(); }

 private:
  vulkan::UniqueHandle<VkCommandBuffer> commandBuffer_;
  VkQueue queue_;
};

template <typename Fn>
void VulkanCommandBuffer::runRenderPass(
    VulkanGraphicsPipeline& graphicsPipeline,
    VulkanFrameBuffer& frameBuffer,
    Fn&& fn) {
  vkResetCommandBuffer(commandBuffer_.get(), 0);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer_.get(), &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to begin recording command buffer"};
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass =
      graphicsPipeline.getRenderPass().getRawRenderPass();
  renderPassInfo.framebuffer = frameBuffer.getRawFrameBuffer();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = frameBuffer.getExtent();

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(
      commandBuffer_.get(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(
      commandBuffer_.get(),
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      graphicsPipeline.getRawPipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(frameBuffer.getExtent().width);
  viewport.height = static_cast<float>(frameBuffer.getExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer_.get(), 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = frameBuffer.getExtent();
  vkCmdSetScissor(commandBuffer_.get(), 0, 1, &scissor);

  fn(commandBuffer_.get());

  vkCmdEndRenderPass(commandBuffer_.get());

  if (vkEndCommandBuffer(commandBuffer_.get()) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to record command buffer"};
  }
}

} // namespace blocks::render
