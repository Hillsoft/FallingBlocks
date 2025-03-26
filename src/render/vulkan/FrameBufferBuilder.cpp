#include "render/vulkan/FrameBufferBuilder.hpp"

#include <cstdint>
#include <span>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

FrameBufferBuilder::FrameBufferBuilder(
    VkRenderPass renderPass,
    std::span<const VkImageView> attachments,
    VkExtent2D extent,
    uint32_t layers)
    : createInfo_() {
  createInfo_.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  createInfo_.pNext = nullptr;
  createInfo_.flags = 0;
  createInfo_.renderPass = renderPass;
  createInfo_.attachmentCount = static_cast<uint32_t>(attachments.size());
  createInfo_.pAttachments = attachments.data();
  createInfo_.width = extent.width;
  createInfo_.height = extent.height;
  createInfo_.layers = layers;
}

UniqueHandle<VkFramebuffer> FrameBufferBuilder::build(VkDevice device) const {
  VkFramebuffer framebuffer;
  if (vkCreateFramebuffer(device, &createInfo_, nullptr, &framebuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to create frame buffer"};
  }
  return UniqueHandle<VkFramebuffer>{framebuffer, device};
}

} // namespace blocks::render::vulkan
