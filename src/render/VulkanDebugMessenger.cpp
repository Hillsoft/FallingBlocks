#include "render/VulkanDebugMessenger.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "render/VulkanInstance.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT /* messageSeverity */,
    VkDebugUtilsMessageTypeFlagsEXT /* messageType */,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /* pUserData */) {
  std::cerr << "Validation layer error: " << pCallbackData->pMessage << "\n";
  std::cerr.flush();
  std::abort();

  return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

} // namespace

VulkanDebugMessenger::VulkanDebugMessenger(VulkanInstance& instance)
    : debugMessenger_(nullptr, nullptr) {
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;

  VkDebugUtilsMessengerEXT debugMessenger = nullptr;
  const VkResult result = createDebugUtilsMessengerEXT(
      instance.getRawInstance(), &createInfo, nullptr, &debugMessenger);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to set up debug messenger"};
  }

  debugMessenger_ = vulkan::UniqueHandle<VkDebugUtilsMessengerEXT>{
      debugMessenger, instance.getRawInstance()};
}

} // namespace blocks::render
