#include "render/VulkanFence.hpp"

#include <cstdint>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanFence::VulkanFence(VulkanGraphicsDevice& device, bool preSignalled)
    : device_(&device), fence_(nullptr, device.getRawDevice()) {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = 0;
  if (preSignalled) {
    fenceInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
  }

  VkFence fence = nullptr;
  VkResult result =
      vkCreateFence(device.getRawDevice(), &fenceInfo, nullptr, &fence);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create fence"};
  }
  fence_ = vulkan::UniqueHandle<VkFence>{fence, device.getRawDevice()};
}

void VulkanFence::wait() const {
  VkFence fence = fence_.get();
  vkWaitForFences(device_->getRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
}

void VulkanFence::reset() {
  VkFence fence = fence_.get();
  vkResetFences(device_->getRawDevice(), 1, &fence);
}

void VulkanFence::waitAndReset() {
  wait();
  reset();
}

} // namespace blocks::render
