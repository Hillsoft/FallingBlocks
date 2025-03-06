#include "render/VulkanInstance.h"

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "render/validationLayers.hpp"

namespace tetris::render {

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

} // namespace

VulkanInstance::VulkanInstance() {
  std::cout << "Initialising Vulkan\n";
  std::cout << "Available Vulkan extensions:\n";
  for (const auto& extension : getSupportedExtensions()) {
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

  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  createInfo.enabledLayerCount = 0;

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to initialise Vulkan instance"};
  }

  std::cout << "Vulkan initialised\n";
}

VulkanInstance::~VulkanInstance() {
  vkDestroyInstance(instance_, nullptr);
}

} // namespace tetris::render
