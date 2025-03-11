#include "render/VulkanUniqueHandleDeleters.hpp"

#include <GLFW/glfw3.h>

#include "util/debug.hpp"

namespace blocks::render::detail {

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
  static auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  } else {
    DEBUG_ASSERT(false);
  }
}

} // namespace blocks::render::detail
