#include "render/VulkanGraphicsDevice.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/validationLayers.hpp"
#include "util/debug.hpp"

namespace tetris::render {

namespace {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies{queueFamilyCount};
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &queueFamilyCount, queueFamilies.data());

  for (int i = 0; i < queueFamilies.size(); i++) {
    if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0) {
      indices.graphicsFamily = i;
    }
  }

  return indices;
}

struct PhysicalDeviceInfo {
  explicit PhysicalDeviceInfo(VkPhysicalDevice device) : device(device) {
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);
    queueFamilies = findQueueFamilies(device);
  }

  VkPhysicalDevice device;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  QueueFamilyIndices queueFamilies;
};

std::vector<VkPhysicalDevice> enumerateDevices(VulkanInstance& instance) {
  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(
      instance.getRawInstance(), &physicalDeviceCount, nullptr);

  std::vector<VkPhysicalDevice> physicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(
      instance.getRawInstance(), &physicalDeviceCount, physicalDevices.data());

  return physicalDevices;
}

bool isDeviceSuitable(const PhysicalDeviceInfo& device) {
  if (!device.features.geometryShader) {
    return false;
  }
  if (!device.queueFamilies.graphicsFamily.has_value()) {
    return false;
  }

  return true;
}

int deviceSuitabilityHeuristic(const PhysicalDeviceInfo& device) {
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

std::unique_ptr<PhysicalDeviceInfo> choosePhysicalDevice(
    VulkanInstance& instance) {
  std::vector<VkPhysicalDevice> devices = enumerateDevices(instance);

  std::vector<std::pair<std::unique_ptr<PhysicalDeviceInfo>, int>>
      rankedDevices;
  rankedDevices.reserve(devices.size());
  for (const auto& device : devices) {
    auto deviceInfo = std::make_unique<PhysicalDeviceInfo>(device);
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

} // namespace

VulkanGraphicsDevice::VulkanGraphicsDevice(VkDevice device) : device_(device) {}

VulkanGraphicsDevice::~VulkanGraphicsDevice() {
  cleanup();
}

VulkanGraphicsDevice::VulkanGraphicsDevice(
    VulkanGraphicsDevice&& other) noexcept
    : device_(other.device_) {
  other.device_ = nullptr;
}

VulkanGraphicsDevice& VulkanGraphicsDevice::operator=(
    VulkanGraphicsDevice&& other) {
  cleanup();
  device_ = other.device_;
  other.device_ = nullptr;

  return *this;
}

void VulkanGraphicsDevice::cleanup() {
  if (device_ != nullptr) {
    vkDestroyDevice(device_, nullptr);
    device_ = nullptr;
  }
}

VulkanGraphicsDevice VulkanGraphicsDevice::make(VulkanInstance& instance) {
  std::unique_ptr<PhysicalDeviceInfo> physicalDevice =
      choosePhysicalDevice(instance);
  DEBUG_ASSERT(physicalDevice != nullptr);
  std::cout << "Using graphics device " << physicalDevice->properties.deviceName
            << "\n";

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex =
      physicalDevice->queueFamilies.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = 0;

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

  return {device};
}

} // namespace tetris::render
