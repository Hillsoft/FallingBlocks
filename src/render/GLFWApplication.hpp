#pragma once

#include <cstdint>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanDebugMessenger.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanInstance.hpp"
#include "render/VulkanPresentStack.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanSurface.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class GLFWApplication {
 public:
  GLFWApplication();
  ~GLFWApplication();

  GLFWApplication(const GLFWApplication& other) = delete;
  GLFWApplication(GLFWApplication&& other) = delete;

  GLFWApplication& operator=(const GLFWApplication& other) = delete;
  GLFWApplication& operator=(GLFWApplication&& other) = delete;

  void run();

  struct PipelineSynchronisationSet {
    vulkan::UniqueHandle<VkSemaphore> imageAvailableSemaphore;
    vulkan::UniqueHandle<VkSemaphore> renderFinishedSemaphore;
    vulkan::UniqueHandle<VkFence> inFlightFence;
  };

 private:
  void drawFrame();

  void onWindowResize();

  void resetSwapChain();

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
  VulkanVertexShader vertexShader_;
  VulkanShader fragmentShader_;
  VulkanGraphicsPipeline pipeline_;
  VulkanPresentStack presentStack_;
  VulkanCommandPool commandPool_;
  std::vector<VulkanCommandBuffer> commandBuffers_;
  std::vector<PipelineSynchronisationSet> synchronisationSets_;
  VulkanBuffer vertexAttributes_;

  bool shouldResetSwapChain_ = 0;
  uint32_t currentFrame_ = 0;
};

} // namespace blocks::render
