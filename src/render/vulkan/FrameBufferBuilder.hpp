#pragma once

#include <cstdint>
#include <span>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class FrameBufferBuilder {
 public:
  FrameBufferBuilder(
      VkRenderPass renderPass,
      std::span<const VkImageView> attachments,
      VkExtent2D extent,
      uint32_t layers);

  UniqueHandle<VkFramebuffer> build(VkDevice device) const;

 private:
  VkFramebufferCreateInfo createInfo_;
};

} // namespace blocks::render::vulkan
