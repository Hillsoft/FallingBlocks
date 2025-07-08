#include "render/VulkanCommandPool.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/debug.hpp"

namespace blocks::render {

VulkanCommandPool::VulkanCommandPool(
    VulkanGraphicsDevice& device, bool loadingQueue)
    : queue_(
          loadingQueue ? device.getGraphicsLoadingQueue()
                       : device.getGraphicsQueue()),
      commandPool_(nullptr, nullptr) {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  DEBUG_ASSERT(device.physicalInfo().queueFamilies.graphicsFamily.has_value());
  poolInfo.queueFamilyIndex =
      *device.physicalInfo().queueFamilies.graphicsFamily;

  VkCommandPool commandPool;
  VkResult result = vkCreateCommandPool(
      device.getRawDevice(), &poolInfo, nullptr, &commandPool);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create command pool"};
  }
  commandPool_ =
      vulkan::UniqueHandle<VkCommandPool>{commandPool, device.getRawDevice()};
}

} // namespace blocks::render
