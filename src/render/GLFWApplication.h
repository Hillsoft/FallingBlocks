#pragma once

#include "render/VulkanGraphicsDevice.h"
#include "render/VulkanInstance.h"
#include "render/glfw_wrapper/Window.h"

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
  glfw::Window window_;
  VulkanInstance vulkan{};
  VulkanGraphicsDevice graphics{};
};

} // namespace tetris::render
