#include "render/vulkan/FenceBuilder.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

FenceBuilder::FenceBuilder()
    : createInfo_(
          {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
           .pNext = nullptr,
           .flags = 0}) {}

FenceBuilder& FenceBuilder::setInitiallySignalled(bool initiallySignalled) {
  if (initiallySignalled) {
    createInfo_.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
  } else {
    createInfo_.flags &= ~VK_FENCE_CREATE_SIGNALED_BIT;
  }
  return *this;
}

UniqueHandle<VkFence> FenceBuilder::build(VkDevice device) const {
  VkFence fence;
  if (vkCreateFence(device, &createInfo_, nullptr, &fence) != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create fence"};
  }
  return UniqueHandle<VkFence>{fence, device};
}

} // namespace blocks::render::vulkan
