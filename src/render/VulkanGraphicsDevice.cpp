#include "render/VulkanGraphicsDevice.hpp"

#include <string.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/validationLayers.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/debug.hpp"

namespace blocks::render {

namespace {

constexpr std::array<const char*, 1> requiredDeviceExtensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VulkanGraphicsDevice::QueueFamilyIndices findQueueFamilies(
    VkInstance instance, VkPhysicalDevice device) {
  VulkanGraphicsDevice::QueueFamilyIndices indices;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies{queueFamilyCount};
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &queueFamilyCount, queueFamilies.data());

  for (int i = 0; i < queueFamilies.size(); i++) {
    if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    presentSupport =
        glfwGetPhysicalDevicePresentationSupport(instance, device, i);
    // vkGetPhysicalDeviceSurfaceSupportKHR(device, i, nullptr,
    // &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }
  }

  return indices;
}

std::vector<VkPhysicalDevice> enumerateDevices(VulkanInstance& instance) {
  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(
      instance.getRawInstance(), &physicalDeviceCount, nullptr);

  std::vector<VkPhysicalDevice> physicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(
      instance.getRawInstance(), &physicalDeviceCount, physicalDevices.data());

  return physicalDevices;
}

bool deviceHasRequiredExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(
      device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions{extensionCount};
  vkEnumerateDeviceExtensionProperties(
      device, nullptr, &extensionCount, availableExtensions.data());

  for (const auto& required : requiredDeviceExtensions) {
    bool found = false;
    for (const auto& available : availableExtensions) {
      if (strcmp(required, available.extensionName) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}

bool isDeviceSuitable(const VulkanGraphicsDevice::PhysicalDeviceInfo& device) {
  if (!device.features.geometryShader) {
    return false;
  }
  if (!device.queueFamilies.graphicsFamily.has_value()) {
    return false;
  }
  if (!device.queueFamilies.presentFamily.has_value()) {
    return false;
  }
  if (!deviceHasRequiredExtensionSupport(device.device)) {
    return false;
  }

  return true;
}

int deviceSuitabilityHeuristic(
    const VulkanGraphicsDevice::PhysicalDeviceInfo& device) {
  if (!isDeviceSuitable(device)) {
    return 0;
  }

  int score = 0;

  if (device.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  // Approximate heuristice to pick between similar devices if there are
  // multiple
  score += device.properties.limits.maxImageDimension2D;

  return score;
}

std::unique_ptr<VulkanGraphicsDevice::PhysicalDeviceInfo> choosePhysicalDevice(
    VulkanInstance& instance) {
  std::vector<VkPhysicalDevice> devices = enumerateDevices(instance);

  std::vector<
      std::pair<std::unique_ptr<VulkanGraphicsDevice::PhysicalDeviceInfo>, int>>
      rankedDevices;
  rankedDevices.reserve(devices.size());
  for (const auto& device : devices) {
    auto deviceInfo =
        std::make_unique<VulkanGraphicsDevice::PhysicalDeviceInfo>(
            instance.getRawInstance(), device);
    int suitabilityHeuristic = deviceSuitabilityHeuristic(*deviceInfo);
    if (suitabilityHeuristic > 0) {
      rankedDevices.emplace_back(std::move(deviceInfo), suitabilityHeuristic);
    }
  }
  std::sort(
      rankedDevices.begin(),
      rankedDevices.end(),
      [](const auto& a, const auto& b) { return a.second > b.second; });

  std::cout << "Available Vulkan devices:\n";
  for (const auto& device : rankedDevices) {
    std::cout << "  " << device.first->properties.deviceName << "\n";
  }

  if (rankedDevices.size() == 0 || rankedDevices[0].second == 0) {
    throw std::runtime_error{"No suitable graphics devices"};
  }

  return std::move(rankedDevices[0].first);
}

struct VkDeviceQueueCreateInfoWrapper {
  VkDeviceQueueCreateInfoWrapper() = default;
  VkDeviceQueueCreateInfoWrapper(const VkDeviceQueueCreateInfoWrapper& other) =
      delete;
  VkDeviceQueueCreateInfoWrapper& operator=(
      const VkDeviceQueueCreateInfoWrapper& other) = delete;
  VkDeviceQueueCreateInfoWrapper(VkDeviceQueueCreateInfoWrapper&& other) =
      delete;
  VkDeviceQueueCreateInfoWrapper& operator=(
      VkDeviceQueueCreateInfoWrapper&& other) = delete;

  std::vector<VkDeviceQueueCreateInfo> createInfo;
  float queuePriority = 1.0f;
};

void makeQueueCreateInfo(
    VkDeviceQueueCreateInfoWrapper& result,
    const VulkanGraphicsDevice::QueueFamilyIndices& indices) {
  DEBUG_ASSERT(result.createInfo.size() == 0);
  DEBUG_ASSERT(indices.graphicsFamily.has_value());
  DEBUG_ASSERT(indices.presentFamily.has_value());
  std::unordered_set<uint32_t> uniqueIndices{
      *indices.graphicsFamily, *indices.presentFamily};

  result.createInfo.reserve(uniqueIndices.size());

  for (uint32_t queueFamily : uniqueIndices) {
    VkDeviceQueueCreateInfo& queueCreateInfo = result.createInfo.emplace_back();
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = queueFamily == *indices.graphicsFamily ? 2 : 1;
    queueCreateInfo.pQueuePriorities = &result.queuePriority;
  }
}

} // namespace

VulkanGraphicsDevice::PhysicalDeviceInfo::PhysicalDeviceInfo(
    VkInstance instance, VkPhysicalDevice device)
    : device(device) {
  vkGetPhysicalDeviceProperties(device, &properties);
  vkGetPhysicalDeviceFeatures(device, &features);
  queueFamilies = findQueueFamilies(instance, device);
}

VulkanGraphicsDevice::VulkanGraphicsDevice(
    vulkan::UniqueHandle<VkDevice> device,
    VkQueue graphicsQueue,
    VkQueue graphicsLoadingQueue,
    VkQueue presentQueue,
    std::unique_ptr<PhysicalDeviceInfo> physicalInfo)
    : device_(std::move(device)),
      graphicsQueue_(graphicsQueue),
      graphicsLoadingQueue_(graphicsLoadingQueue),
      presentQueue_(presentQueue),
      physicalInfo_(std::move(physicalInfo)) {}

VulkanGraphicsDevice VulkanGraphicsDevice::make(VulkanInstance& instance) {
  std::unique_ptr<PhysicalDeviceInfo> physicalDevice =
      choosePhysicalDevice(instance);
  DEBUG_ASSERT(physicalDevice != nullptr);
  std::cout << "Using graphics device " << physicalDevice->properties.deviceName
            << "\n";

  VkDeviceQueueCreateInfoWrapper queueCreateInfo{};
  makeQueueCreateInfo(queueCreateInfo, physicalDevice->queueFamilies);

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfo.createInfo.data();
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfo.createInfo.size());
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(requiredDeviceExtensions.size());
  createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

  if (kEnableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(kValidationLayers.size());
    createInfo.ppEnabledLayerNames = kValidationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkDevice device;
  VkResult result =
      vkCreateDevice(physicalDevice->device, &createInfo, nullptr, &device);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create logical device"};
  }

  VkQueue graphicsQueue;
  vkGetDeviceQueue(
      device,
      physicalDevice->queueFamilies.graphicsFamily.value(),
      0,
      &graphicsQueue);
  VkQueue graphicsLoadingQueue;
  vkGetDeviceQueue(
      device,
      physicalDevice->queueFamilies.graphicsFamily.value(),
      1,
      &graphicsLoadingQueue);
  VkQueue presentQueue;
  vkGetDeviceQueue(
      device,
      physicalDevice->queueFamilies.presentFamily.value(),
      0,
      &presentQueue);

  return {
      vulkan::UniqueHandle<VkDevice>{device},
      graphicsQueue,
      graphicsLoadingQueue,
      presentQueue,
      std::move(physicalDevice)};
}

} // namespace blocks::render
