#pragma once

#include "render/VulkanGraphicsDevice.h"
#include "render/VulkanInstance.h"
#include "render/VulkanSurface.h"

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
  VulkanGraphicsDevice graphics_;
  VulkanSurface surface_;
};

} // namespace tetris::render
