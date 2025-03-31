#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class RenderPassBuilder {
 public:
  RenderPassBuilder();

  RenderPassBuilder& addAttachment(const VkAttachmentDescription& attachment);
  VkAttachmentReference addAttachmentGetDescription(
      const VkAttachmentDescription& attachment, VkImageLayout inFlightLayout);

  RenderPassBuilder& addSubpass(
      VkPipelineBindPoint bindPoint,
      std::span<const VkAttachmentReference> inputAttachments,
      std::span<const VkAttachmentReference> colorAttachments);
  uint32_t addSubpassGetIndex(
      VkPipelineBindPoint bindPoint,
      std::span<const VkAttachmentReference> inputAttachments,
      std::span<const VkAttachmentReference> colorAttachments);

  RenderPassBuilder& addSubpassDependency(
      const VkSubpassDependency& dependency);

  UniqueHandle<VkRenderPass> build(VkDevice device) const;

 private:
  std::vector<VkAttachmentDescription> attachments_;
  std::vector<VkSubpassDescription> subpasses_;
  std::vector<VkSubpassDependency> dependencies_;
  VkRenderPassCreateInfo createInfo_;
};

} // namespace blocks::render::vulkan
