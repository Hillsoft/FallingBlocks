#pragma once

#include <cstdint>
#include "render/RenderSubSystem.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanPresentStack.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanSurface.hpp"
#include "render/VulkanTexture.hpp"

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

 private:
  void drawFrame();

  void onWindowResize();

  void resetSwapChain();

  struct GLFWLifetimeScope {
    GLFWLifetimeScope();
    ~GLFWLifetimeScope();
  };

  [[no_unique_address]] GLFWLifetimeScope lifetimeScope_;
  RenderSubSystem render_;
  VulkanSurface surface_;
  VulkanVertexShader vertexShader_;
  VulkanShader fragmentShader_;
  VulkanDescriptorSetLayout descriptorSetLayout_;
  VulkanDescriptorPool descriptorPool_;
  VulkanGraphicsPipeline pipeline_;
  VulkanPresentStack presentStack_;
  VulkanBuffer vertexAttributes_;
  VulkanTexture texture_;

  bool shouldResetSwapChain_ = 0;
  uint32_t currentFrame_ = 0;
};

} // namespace blocks::render
