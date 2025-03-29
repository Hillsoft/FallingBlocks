#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanCommandBuffer.hpp"
#include "render/VulkanCommandPool.hpp"
#ifndef NDEBUG
#include "render/VulkanDebugMessenger.hpp"
#endif
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanInstance.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class GLFWApplication;

class RenderSubSystem {
 public:
  RenderSubSystem();

  struct PipelineSynchronisationSet {
    vulkan::UniqueHandle<VkSemaphore> imageAvailableSemaphore;
    vulkan::UniqueHandle<VkSemaphore> renderFinishedSemaphore;
    vulkan::UniqueHandle<VkFence> inFlightFence;
  };

 private:
  struct GLFWLifetimeScope {
    GLFWLifetimeScope();
    ~GLFWLifetimeScope();
  };

  [[no_unique_address]] GLFWLifetimeScope lifetimeScope_;
  VulkanInstance instance_;
#ifndef NDEBUG
  VulkanDebugMessenger debugMessenger_;
#endif
  VulkanGraphicsDevice graphics_;
  VulkanCommandPool commandPool_;
  std::vector<VulkanCommandBuffer> commandBuffers_;
  std::vector<PipelineSynchronisationSet> synchronisationSets_;

 public:
  friend class GLFWApplication;
};

} // namespace blocks::render
