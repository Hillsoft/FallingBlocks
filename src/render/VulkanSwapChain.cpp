#include "render/VulkanSwapChain.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>

#include "render/glfw_wrapper/Window.hpp"
#include "util/debug.hpp"

namespace tetris::render {

namespace {

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

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) {
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  // Guaranteed to always be availabe
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(
    glfw::Window& window, const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    // Required to use given extent
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window.getRawWindow(), &width, &height);

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

} // namespace

VulkanSwapChain::VulkanSwapChain(
    VulkanSurface& surface, VulkanGraphicsDevice& graphicsDevice)
    : graphicsDevice_(&graphicsDevice) {
  const VulkanGraphicsDevice::SwapChainSupportDetails& swapChainSupport =
      graphicsDevice.physicalInfo().swapChainSupport;

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent =
      chooseSwapExtent(surface.window(), swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface.getRawSurface();
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

  swapChain_ = swapChain;

  vkGetSwapchainImagesKHR(
      graphicsDevice.getRawDevice(), swapChain, &imageCount, nullptr);
  swapChainImages_.resize(imageCount);
  vkGetSwapchainImagesKHR(
      graphicsDevice.getRawDevice(),
      swapChain,
      &imageCount,
      swapChainImages_.data());
}

VulkanSwapChain::VulkanSwapChain(VulkanSwapChain&& other) noexcept
    : graphicsDevice_(other.graphicsDevice_), swapChain_(other.swapChain_) {
  other.swapChain_ = nullptr;
}

VulkanSwapChain& VulkanSwapChain::operator=(VulkanSwapChain&& other) noexcept {
  std::swap(graphicsDevice_, other.graphicsDevice_);
  std::swap(swapChain_, other.swapChain_);

  return *this;
}

VulkanSwapChain::~VulkanSwapChain() {
  if (swapChain_ != nullptr) {
    vkDestroySwapchainKHR(graphicsDevice_->getRawDevice(), swapChain_, nullptr);
  }
}

} // namespace tetris::render
