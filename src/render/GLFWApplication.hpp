#pragma once

#include "render/VulkanGraphicsDevice.hpp"
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
  VulkanSurface surface_;
  VulkanGraphicsDevice graphics_;
  VulkanSwapChain swapChain_;
};

} // namespace tetris::render
