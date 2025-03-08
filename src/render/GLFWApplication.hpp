#pragma once

#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanDebugMessenger.hpp"
#include "render/VulkanFence.hpp"
#include "render/VulkanFrameBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanInstance.hpp"
#include "render/VulkanSemaphore.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanSurface.hpp"
#include "render/VulkanSwapChain.hpp"

namespace tetris::render {

class GLFWApplication {
 public:
  GLFWApplication();
  ~GLFWApplication();

  GLFWApplication(const GLFWApplication& other) = delete;
  GLFWApplication(GLFWApplication&& other) = delete;

  GLFWApplication& operator=(const GLFWApplication& other) = delete;
  GLFWApplication& operator=(GLFWApplication&& other) = delete;

  void run();

 private:
  struct GLFWLifetimeScope {
    GLFWLifetimeScope();
    ~GLFWLifetimeScope();
  };

  [[no_unique_address]] GLFWLifetimeScope lifetimeScope_;
  VulkanInstance vulkan_{};
#ifndef NDEBUG
  VulkanDebugMessenger debugMessenger_;
#endif
  VulkanSurface surface_;
  VulkanGraphicsDevice graphics_;
  VulkanSwapChain swapChain_;
  std::vector<VulkanImageView> swapChainImageViews_;
  VulkanShader vertexShader_;
  VulkanShader fragmentShader_;
  VulkanGraphicsPipeline pipeline_;
  std::vector<VulkanFrameBuffer> frameBuffers_;
  VulkanCommandPool commandPool_;
  VulkanCommandBuffer commandBuffer_;
  VulkanSemaphore imageAvailableSemaphore_;
  VulkanSemaphore renderFinishedSemaphore_;
  VulkanFence inFlightFence_;
};

} // namespace tetris::render
