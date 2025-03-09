#include "render/VulkanCommandPool.hpp"

#include <stdexcept>

#include "util/debug.hpp"

namespace blocks::render {

VulkanCommandPool::VulkanCommandPool(VulkanGraphicsDevice& device)
    : device_(&device),
      queue_(device.getGraphicsQueue()),
      commandPool_(nullptr) {
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
    : device_(other.device_),
      queue_(other.queue_),
      commandPool_(other.commandPool_) {
  other.device_ = nullptr;
  other.queue_ = nullptr;
  other.commandPool_ = nullptr;
}

VulkanCommandPool& VulkanCommandPool::operator=(
    VulkanCommandPool&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(queue_, other.queue_);
  std::swap(commandPool_, other.commandPool_);

  return *this;
}

} // namespace blocks::render
