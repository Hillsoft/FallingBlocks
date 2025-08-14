#include "render/VulkanSwapChain.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/ImageViewBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/debug.hpp"

namespace blocks::render {

namespace {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
  VkSurfaceFormatKHR preferredFormat{};
};

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) {
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
      return availablePresentMode;
    }
  }

  // Guaranteed to always be availabe
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(
    GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    // Required to use given extent
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(
        actualExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  DEBUG_ASSERT(availableFormats.size() > 0);
  return availableFormats[0];
}

SwapChainSupportDetails getSwapChainSupportDetails(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  details.preferredFormat = chooseSwapSurfaceFormat(details.formats);

  if (details.preferredFormat.format != VK_FORMAT_B8G8R8A8_SRGB) {
    throw std::runtime_error{"Required surface format unsupported"};
  }

  return details;
}

} // namespace

VulkanSwapChain::VulkanSwapChain(
    GLFWwindow* window,
    VkSurfaceKHR surface,
    VulkanGraphicsDevice& graphicsDevice)
    : graphicsDevice_(&graphicsDevice),
      swapChain_(nullptr, nullptr),
      queue_(graphicsDevice.getPresentQueue()) {
  const SwapChainSupportDetails swapChainSupport =
      getSwapChainSupportDetails(graphicsDevice.physicalInfo().device, surface);

  VkSurfaceFormatKHR surfaceFormat = swapChainSupport.preferredFormat;
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const VulkanGraphicsDevice::QueueFamilyIndices& queueFamilies =
      graphicsDevice.physicalInfo().queueFamilies;
  std::array<uint32_t, 2> queueFamilyIndices{
      queueFamilies.graphicsFamily.value(),
      queueFamilies.presentFamily.value()};

  if (queueFamilies.graphicsFamily.value() !=
      queueFamilies.presentFamily.value()) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swapChain;
  VkResult result = vkCreateSwapchainKHR(
      graphicsDevice.getRawDevice(), &createInfo, nullptr, &swapChain);
  if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to create swap chain!"};
  }

  swapChain_ = vulkan::UniqueHandle<VkSwapchainKHR>{
      swapChain, graphicsDevice.getRawDevice()};
  extent_ = extent;
  format_ = surfaceFormat.format;

  vkGetSwapchainImagesKHR(
      graphicsDevice.getRawDevice(), swapChain, &imageCount, nullptr);
  swapChainImages_.resize(imageCount);
  vkGetSwapchainImagesKHR(
      graphicsDevice.getRawDevice(),
      swapChain,
      &imageCount,
      swapChainImages_.data());
}

std::vector<vulkan::UniqueHandle<VkImageView>> VulkanSwapChain::getImageViews()
    const {
  std::vector<vulkan::UniqueHandle<VkImageView>> result;
  result.reserve(swapChainImages_.size());

  for (const auto& swapImage : swapChainImages_) {
    result.emplace_back(
        vulkan::ImageViewBuilder(swapImage, format_)
            .build(graphicsDevice_->getRawDevice()));
  }

  return result;
}

std::optional<uint32_t> VulkanSwapChain::getNextImageIndex(
    VkSemaphore semaphore, VkFence fence) {
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      graphicsDevice_->getRawDevice(),
      swapChain_.get(),
      UINT64_MAX,
      semaphore,
      fence,
      &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return std::nullopt;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error{"Failed to acquire swap chain image"};
  }

  return imageIndex;
}

void VulkanSwapChain::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
  VkSwapchainKHR swapChain = swapChain_.get();
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  if (waitSemaphore != nullptr) {
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;
  }
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapChain;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  vkQueuePresentKHR(queue_, &presentInfo);
}

} // namespace blocks::render
