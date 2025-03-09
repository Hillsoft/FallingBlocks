#include "render/VulkanSemaphore.hpp"

#include <stdexcept>

namespace blocks::render {

VulkanSemaphore::VulkanSemaphore(VulkanGraphicsDevice& device)
    : device_(&device), semaphore_(nullptr) {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkResult result = vkCreateSemaphore(
      device.getRawDevice(), &semaphoreInfo, nullptr, &semaphore_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create semaphore"};
  }
}

VulkanSemaphore::~VulkanSemaphore() {
  if (semaphore_ != nullptr) {
    vkDestroySemaphore(device_->getRawDevice(), semaphore_, nullptr);
  }
}

VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& other) noexcept
    : device_(other.device_), semaphore_(other.semaphore_) {
  other.device_ = nullptr;
  other.semaphore_ = nullptr;
}

VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(semaphore_, other.semaphore_);

  return *this;
}

} // namespace blocks::render
