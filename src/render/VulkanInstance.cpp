#include "render/VulkanInstance.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "log/Logger.hpp"
#include "render/validationLayers.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/string.hpp"

namespace blocks::render {

namespace {

bool checkValidationLayers() {
  // Check supported
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers{layerCount};
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* requestedLayer : kValidationLayers) {
    bool layerFound = false;

    for (const auto& layerProperty : availableLayers) {
      if (std::strcmp(
              static_cast<const char*>(layerProperty.layerName),
              requestedLayer) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

std::vector<VkExtensionProperties> getSupportedExtensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions{extensionCount};
  vkEnumerateInstanceExtensionProperties(
      nullptr, &extensionCount, extensions.data());

  return extensions;
}

bool validateRequiredExtensions(
    const std::vector<const char*>& requiredExtensions,
    const std::vector<VkExtensionProperties>& availableExtensions =
        getSupportedExtensions()) {
  for (const char* requested : requiredExtensions) {
    bool extensionFound = false;

    for (const auto& available : availableExtensions) {
      if (std::strcmp(
              static_cast<const char*>(available.extensionName), requested) ==
          0) {
        extensionFound = true;
        break;
      }
    }

    if (!extensionFound) {
      return false;
    }
  }

  return true;
}

} // namespace

VulkanInstance::VulkanInstance() : instance_(nullptr) {
  log::LoggerSystem::logToDefault(log::LogLevel::INFO, "Initialising Vulkan");
  std::string availableExtensionsString = "Available Vulkan extensions:\n";
  const auto supportedExtensions = getSupportedExtensions();
  for (const auto& extension : supportedExtensions) {
    availableExtensionsString += util::toString(
        "  ", static_cast<const char*>(extension.extensionName), "\n");
  }
  log::LoggerSystem::logToDefault(
      log::LogLevel::INFO,
      std::string_view{availableExtensionsString}.substr(
          0, availableExtensionsString.size() - 1));

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "FallingBlocks";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  if (kEnableValidationLayers) {
    log::LoggerSystem::logToDefault(
        log::LogLevel::INFO, "Using validation layers");
    if (checkValidationLayers()) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(kValidationLayers.size());
      createInfo.ppEnabledLayerNames = kValidationLayers.data();
    } else {
      log::LoggerSystem::logToDefault(
          log::LogLevel::WARNING,
          "Requested validation layers not available, validation will not be used");
    }
  }

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions{
      glfwExtensions,
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      glfwExtensions + glfwExtensionCount};
  if (kEnableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  if (!validateRequiredExtensions(extensions, supportedExtensions)) {
    throw std::runtime_error{"Not all required extensions are available"};
  }

  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkInstance instance = nullptr;
  const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to initialise Vulkan instance"};
  }
  instance_ = vulkan::UniqueHandle<VkInstance>{instance};

  log::LoggerSystem::logToDefault(log::LogLevel::INFO, "Vulkan initialised");
}

} // namespace blocks::render
