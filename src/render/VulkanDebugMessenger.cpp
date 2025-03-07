#include "render/VulkanDebugMessenger.hpp"

#include <iostream>

#include "util/debug.hpp"

namespace tetris::render {

namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
  std::cerr << "Validation layer error: " << pCallbackData->pMessage
            << std::endl;

  return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

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

} // namespace

VulkanDebugMessenger::VulkanDebugMessenger(VulkanInstance& instance)
    : instance_(&instance), debugMessenger_(nullptr) {
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;

  VkResult result = createDebugUtilsMessengerEXT(
      instance.getRawInstance(), &createInfo, nullptr, &debugMessenger_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to set up debug messenger"};
  }
}

VulkanDebugMessenger::~VulkanDebugMessenger() {
  if (debugMessenger_ != nullptr) {
    destroyDebugUtilsMessengerEXT(
        instance_->getRawInstance(), debugMessenger_, nullptr);
  }
}

VulkanDebugMessenger::VulkanDebugMessenger(
    VulkanDebugMessenger&& other) noexcept
    : instance_(other.instance_), debugMessenger_(other.debugMessenger_) {
  other.instance_ = nullptr;
  other.debugMessenger_ = nullptr;
}

VulkanDebugMessenger& VulkanDebugMessenger::operator=(
    VulkanDebugMessenger&& other) noexcept {
  std::swap(instance_, other.instance_);
  std::swap(debugMessenger_, other.debugMessenger_);

  return *this;
}

} // namespace tetris::render
