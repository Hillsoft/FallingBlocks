#pragma once

#include "render/VulkanDebugMessenger.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/VulkanInstance.hpp"
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
};

} // namespace tetris::render
