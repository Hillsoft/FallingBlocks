#include "render/vulkan/RenderPassBuilder.hpp"

#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

RenderPassBuilder::RenderPassBuilder() {
  createInfo_.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo_.pNext = nullptr;
  createInfo_.flags = 0;
  createInfo_.attachmentCount = 0;
  createInfo_.pAttachments = nullptr;
  createInfo_.subpassCount = 0;
  createInfo_.pSubpasses = nullptr;
  createInfo_.dependencyCount = 0;
  createInfo_.pDependencies = nullptr;
}

RenderPassBuilder& RenderPassBuilder::addAttachment(
    const VkAttachmentDescription& attachment) {
  attachments_.emplace_back(attachment);
  createInfo_.attachmentCount = static_cast<uint32_t>(attachments_.size());
  createInfo_.pAttachments = attachments_.data();

  return *this;
}

VkAttachmentReference RenderPassBuilder::addAttachmentGetDescription(
    const VkAttachmentDescription& attachment, VkImageLayout inFlightLayout) {
  uint32_t index = static_cast<uint32_t>(attachments_.size());
  addAttachment(attachment);
  return VkAttachmentReference{.attachment = index, .layout = inFlightLayout};
}

RenderPassBuilder& RenderPassBuilder::addSubpass(
    VkPipelineBindPoint bindPoint,
    std::span<const VkAttachmentReference> inputAttachments,
    std::span<const VkAttachmentReference> colorAttachments) {
  VkSubpassDescription subpass{};
  subpass.flags = 0;
  subpass.pipelineBindPoint = bindPoint;
  subpass.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
  subpass.pInputAttachments = inputAttachments.data();
  subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
  subpass.pColorAttachments = colorAttachments.data();
  subpass.pResolveAttachments = nullptr;
  subpass.pDepthStencilAttachment = nullptr;
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = nullptr;

  subpasses_.emplace_back(subpass);
  createInfo_.subpassCount = static_cast<uint32_t>(subpasses_.size());
  createInfo_.pSubpasses = subpasses_.data();

  return *this;
}

uint32_t RenderPassBuilder::addSubpassGetIndex(
    VkPipelineBindPoint bindPoint,
    std::span<const VkAttachmentReference> inputAttachments,
    std::span<const VkAttachmentReference> colorAttachments) {
  uint32_t index = static_cast<uint32_t>(subpasses_.size());
  addSubpass(bindPoint, inputAttachments, colorAttachments);
  return index;
}

RenderPassBuilder& RenderPassBuilder::addSubpassDependency(
    const VkSubpassDependency& dependency) {
  dependencies_.emplace_back(dependency);
  createInfo_.dependencyCount = static_cast<uint32_t>(dependencies_.size());
  createInfo_.pDependencies = dependencies_.data();
  return *this;
}

UniqueHandle<VkRenderPass> RenderPassBuilder::build(VkDevice device) const {
  VkRenderPass renderPass;
  if (vkCreateRenderPass(device, &createInfo_, nullptr, &renderPass) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to create render pass"};
  }
  return UniqueHandle<VkRenderPass>(renderPass, device);
}

} // namespace blocks::render::vulkan
