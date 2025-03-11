#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/VulkanSurface.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanGraphicsDevice {
 public:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    VkSurfaceFormatKHR preferredFormat{};
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

  static VulkanGraphicsDevice make(
      VulkanInstance& instance, VulkanSurface& surface);

  const PhysicalDeviceInfo& physicalInfo() const { return *physicalInfo_; }

  VkDevice getRawDevice() { return device_.get(); }
  VkQueue getGraphicsQueue() { return graphicsQueue_; }
  VkQueue getPresentQueue() { return presentQueue_; }

 private:
  VulkanGraphicsDevice(
      vulkan::UniqueHandle<VkDevice> device,
      VkQueue graphicsQueue,
      VkQueue presentQueue,
      std::unique_ptr<PhysicalDeviceInfo> physicalInfo);

  vulkan::UniqueHandle<VkDevice> device_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  std::unique_ptr<PhysicalDeviceInfo> physicalInfo_;
};

} // namespace blocks::render
