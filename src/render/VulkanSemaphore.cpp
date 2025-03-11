#include "render/VulkanSemaphore.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

VulkanSemaphore::VulkanSemaphore(VulkanGraphicsDevice& device)
    : semaphore_(nullptr, nullptr) {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkSemaphore semaphore;
  VkResult result = vkCreateSemaphore(
      device.getRawDevice(), &semaphoreInfo, nullptr, &semaphore);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create semaphore"};
  }

  semaphore_ =
      vulkan::UniqueHandle<VkSemaphore>{semaphore, device.getRawDevice()};
}

} // namespace blocks::render
