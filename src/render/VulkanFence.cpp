#include "render/VulkanFence.hpp"

#include <stdexcept>

namespace tetris::render {

VulkanFence::VulkanFence(VulkanGraphicsDevice& device, bool preSignalled)
    : device_(&device), fence_(nullptr) {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = 0;
  if (preSignalled) {
    fenceInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
  }

  VkResult result =
      vkCreateFence(device.getRawDevice(), &fenceInfo, nullptr, &fence_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create fence"};
  }
}

VulkanFence::~VulkanFence() {
  if (fence_ != nullptr) {
    vkDestroyFence(device_->getRawDevice(), fence_, nullptr);
  }
}

VulkanFence::VulkanFence(VulkanFence&& other) noexcept
    : device_(other.device_), fence_(other.fence_) {
  other.device_ = nullptr;
  other.fence_ = nullptr;
}

VulkanFence& VulkanFence::operator=(VulkanFence&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(fence_, other.fence_);

  return *this;
}

void VulkanFence::wait() const {
  vkWaitForFences(device_->getRawDevice(), 1, &fence_, VK_TRUE, UINT64_MAX);
}

void VulkanFence::reset() {
  vkResetFences(device_->getRawDevice(), 1, &fence_);
}

void VulkanFence::waitAndReset() {
  wait();
  reset();
}

} // namespace tetris::render
