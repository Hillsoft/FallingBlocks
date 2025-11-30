#include "render/VulkanPresentStack.hpp"

#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "log/Logger.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/glfw_wrapper/Window.hpp"
#include "render/vulkan/FrameBufferBuilder.hpp"
#include "render/vulkan/SurfaceBuilder.hpp"
#include "render/vulkan/UniqueHandle.hpp"
#include "util/resettable.hpp"

namespace blocks::render {

namespace {

std::vector<vulkan::UniqueHandle<VkFramebuffer>> makeFrameBuffers(
    VulkanGraphicsDevice& device,
    VkRenderPass renderPass,
    std::vector<vulkan::UniqueHandle<VkImageView>>& imageViews,
    VkExtent2D extent) {
  std::vector<vulkan::UniqueHandle<VkFramebuffer>> result;
  result.reserve(imageViews.size());

  for (auto& v : imageViews) {
    result.emplace_back(
        vulkan::FrameBufferBuilder(
            renderPass, std::span{&v.get(), 1}, extent, 1)
            .build(device.getRawDevice()));
  }

  return result;
}

} // namespace

VulkanPresentStack::VulkanPresentStack(
    VkInstance instance,
    VulkanGraphicsDevice& device,
    glfw::Window window,
    VkRenderPass renderPass)
    : window_(std::move(window)),
      surface_(
          vulkan::createSurfaceForWindow(instance, window_.getRawWindow())),
      renderPass_(renderPass),
      swapChainData_(
          device, window_.getRawWindow(), surface_.get(), renderPass),
      device_(&device) {}

VulkanPresentStack::SwapChainData::SwapChainData(
    VulkanGraphicsDevice& device,
    GLFWwindow* window,
    VkSurfaceKHR surface,
    VkRenderPass renderPass)
    : swapChain(window, surface, device),
      imageViews(swapChain.getImageViews()),
      frameBuffer(makeFrameBuffers(
          device, renderPass, imageViews, swapChain.getSwapchainExtent())) {}

VulkanPresentStack::FrameData VulkanPresentStack::getNextImageIndex(
    VkSemaphore semaphore, VkFence fence) {
  std::optional<uint32_t> nextImageIndex =
      swapChainData_->swapChain.getNextImageIndex(semaphore, fence);
  if (nextImageIndex.has_value()) {
    return FrameData{FrameDataCreationTag{}, *nextImageIndex, this};
  } else {
    return FrameData{FrameDataCreationTag{}};
  }
}

void VulkanPresentStack::reset() {
  log::LoggerSystem::logToDefault(log::LogLevel::INFO, "Resetting swap chain");
  vkDeviceWaitIdle(device_->getRawDevice());
  swapChainData_.reset(
      *device_, window_.getRawWindow(), surface_.get(), renderPass_);
}

VulkanPresentStack::FrameData::FrameData(
    FrameDataCreationTag /* tag */,
    uint32_t imageIndex,
    VulkanPresentStack* owner)
    : refreshSwapChainRequired_(false),
      imageIndex_(imageIndex),
      owner_(owner) {}

VulkanPresentStack::FrameData::FrameData(FrameDataCreationTag /* tag */)
    : refreshSwapChainRequired_(true), imageIndex_(0), owner_(nullptr) {}

} // namespace blocks::render
