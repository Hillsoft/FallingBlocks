#include "render/VulkanGraphicsDevice.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <unordered_set>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/validationLayers.hpp"
#include "util/debug.hpp"

namespace tetris::render {

namespace {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
};

QueueFamilyIndices findQueueFamilies(
    VkPhysicalDevice device, VulkanSurface& surface) {
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
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        device, i, surface.getRawSurface(), &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }
  }

  return indices;
}

struct PhysicalDeviceInfo {
  explicit PhysicalDeviceInfo(VkPhysicalDevice device, VulkanSurface& surface)
      : device(device) {
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);
    queueFamilies = findQueueFamilies(device, surface);
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
  if (!device.queueFamilies.presentFamily.has_value()) {
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
    VulkanInstance& instance, VulkanSurface& surface) {
  std::vector<VkPhysicalDevice> devices = enumerateDevices(instance);

  std::vector<std::pair<std::unique_ptr<PhysicalDeviceInfo>, int>>
      rankedDevices;
  rankedDevices.reserve(devices.size());
  for (const auto& device : devices) {
    auto deviceInfo = std::make_unique<PhysicalDeviceInfo>(device, surface);
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
    VkDeviceQueueCreateInfoWrapper& result, const QueueFamilyIndices& indices) {
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
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &result.queuePriority;
  }
}

} // namespace

VulkanGraphicsDevice::VulkanGraphicsDevice(
    VkDevice device, VkQueue graphicsQueue, VkQueue presentQueue)
    : device_(device),
      graphicsQueue_(graphicsQueue),
      presentQueue_(presentQueue) {}

VulkanGraphicsDevice::~VulkanGraphicsDevice() {
  cleanup();
}

VulkanGraphicsDevice::VulkanGraphicsDevice(
    VulkanGraphicsDevice&& other) noexcept
    : device_(other.device_),
      graphicsQueue_(other.graphicsQueue_),
      presentQueue_(other.presentQueue_) {
  other.device_ = nullptr;
  other.graphicsQueue_ = nullptr;
  other.presentQueue_ = nullptr;
}

VulkanGraphicsDevice& VulkanGraphicsDevice::operator=(
    VulkanGraphicsDevice&& other) noexcept {
  std::swap(device_, other.device_);
  std::swap(graphicsQueue_, other.graphicsQueue_);
  std::swap(presentQueue_, other.presentQueue_);

  return *this;
}

void VulkanGraphicsDevice::cleanup() {
  if (device_ != nullptr) {
    vkDestroyDevice(device_, nullptr);
    device_ = nullptr;
  }
}

VulkanGraphicsDevice VulkanGraphicsDevice::make(
    VulkanInstance& instance, VulkanSurface& surface) {
  std::unique_ptr<PhysicalDeviceInfo> physicalDevice =
      choosePhysicalDevice(instance, surface);
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

  VkQueue graphicsQueue;
  vkGetDeviceQueue(
      device,
      physicalDevice->queueFamilies.graphicsFamily.value(),
      0,
      &graphicsQueue);
  VkQueue presentQueue;
  vkGetDeviceQueue(
      device,
      physicalDevice->queueFamilies.presentFamily.value(),
      0,
      &presentQueue);

  return {device, graphicsQueue, presentQueue};
}

} // namespace tetris::render
