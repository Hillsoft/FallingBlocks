#include "render/VulkanInstance.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "render/validationLayers.hpp"

namespace blocks::render {

namespace {

bool checkValidationLayers() {
  // Check supported
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers{layerCount};
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* requestedLayer : kValidationLayers) {
    bool layerFound = false;

    for (const auto& layerProperty : availableLayers) {
      if (strcmp(layerProperty.layerName, requestedLayer) == 0) {
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
      if (strcmp(available.extensionName, requested) == 0) {
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

VulkanInstance::VulkanInstance() {
  std::cout << "Initialising Vulkan\n";
  std::cout << "Available Vulkan extensions:\n";
  const auto supportedExtensions = getSupportedExtensions();
  for (const auto& extension : supportedExtensions) {
    std::cout << "  " << extension.extensionName << "\n";
  }

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

  if constexpr (kEnableValidationLayers) {
    std::cout << "Using validation layers\n";
    if (checkValidationLayers()) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(kValidationLayers.size());
      createInfo.ppEnabledLayerNames = kValidationLayers.data();
    } else {
      std::cout
          << "WARNING: requested validation layers not available, validation will not be used\n";
    }
  }

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions{
      glfwExtensions, glfwExtensions + glfwExtensionCount};
  if (kEnableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  if (!validateRequiredExtensions(extensions, supportedExtensions)) {
    throw std::runtime_error{"Not all required extensions are available"};
  }

  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to initialise Vulkan instance"};
  }

  std::cout << "Vulkan initialised\n";
}

VulkanInstance::~VulkanInstance() {
  vkDestroyInstance(instance_, nullptr);
}

} // namespace blocks::render
