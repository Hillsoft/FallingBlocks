#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/VulkanSurface.hpp"

namespace blocks::render {

class VulkanGraphicsDevice {
 public:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    VkSurfaceFormatKHR preferredFormat;
  };

  struct PhysicalDeviceInfo {
    explicit PhysicalDeviceInfo(
        VkPhysicalDevice device, VulkanSurface& surface);

    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    QueueFamilyIndices queueFamilies;
    SwapChainSupportDetails swapChainSupport;
  };

  ~VulkanGraphicsDevice();

  VulkanGraphicsDevice(const VulkanGraphicsDevice& other) = delete;
  VulkanGraphicsDevice& operator=(const VulkanGraphicsDevice& other) = delete;

  VulkanGraphicsDevice(VulkanGraphicsDevice&& other) noexcept;
  VulkanGraphicsDevice& operator=(VulkanGraphicsDevice&& other) noexcept;

  static VulkanGraphicsDevice make(
      VulkanInstance& instance, VulkanSurface& surface);

  const PhysicalDeviceInfo& physicalInfo() const { return *physicalInfo_; }

  VkDevice getRawDevice() { return device_; }
  VkQueue getGraphicsQueue() { return graphicsQueue_; }
  VkQueue getPresentQueue() { return presentQueue_; }

 private:
  VulkanGraphicsDevice(
      VkDevice device,
      VkQueue graphicsQueue,
      VkQueue presentQueue,
      std::unique_ptr<PhysicalDeviceInfo> physicalInfo);

  void cleanup();

  VkDevice device_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  std::unique_ptr<PhysicalDeviceInfo> physicalInfo_;
};

} // namespace blocks::render
