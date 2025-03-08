#include "render/VulkanCommandPool.hpp"

#include "util/debug.hpp"

namespace tetris::render {

VulkanCommandPool::VulkanCommandPool(VulkanGraphicsDevice& device)
    : device_(&device), commandPool_(nullptr) {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  DEBUG_ASSERT(device.physicalInfo().queueFamilies.graphicsFamily.has_value());
  poolInfo.queueFamilyIndex =
      *device.physicalInfo().queueFamilies.graphicsFamily;

  VkResult result = vkCreateCommandPool(
      device.getRawDevice(), &poolInfo, nullptr, &commandPool_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create command pool"};
  }
}

VulkanCommandPool::~VulkanCommandPool() {
  if (commandPool_ != nullptr) {
    vkDestroyCommandPool(device_->getRawDevice(), commandPool_, nullptr);
  }
}

VulkanCommandPool::VulkanCommandPool(VulkanCommandPool&& other) noexcept
    : device_(other.device_), commandPool_(other.commandPool_) {
  other.device_ = nullptr;
  other.commandPool_ = nullptr;
}

VulkanCommandPool& VulkanCommandPool::operator=(
    VulkanCommandPool&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(commandPool_, other.commandPool_);

  return *this;
}

} // namespace tetris::render
