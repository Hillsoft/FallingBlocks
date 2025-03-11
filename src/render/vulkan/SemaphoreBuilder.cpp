#include "render/vulkan/SemaphoreBuilder.hpp"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

SemaphoreBuilder::SemaphoreBuilder() : createInfo_() {
  createInfo_.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
}

UniqueHandle<VkSemaphore> SemaphoreBuilder::build(VkDevice device) const {
  VkSemaphore semaphore{};
  if (vkCreateSemaphore(device, &createInfo_, nullptr, &semaphore) !=
      VK_SUCCESS) {
    throw std::runtime_error{"Failed to create semaphore"};
  }
  return UniqueHandle<VkSemaphore>{semaphore, device};
}

} // namespace blocks::render::vulkan
