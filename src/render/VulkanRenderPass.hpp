#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

class VulkanRenderPass {
 public:
  VulkanRenderPass(VulkanGraphicsDevice& device, VkFormat imageFormat);
  ~VulkanRenderPass();

  VulkanRenderPass(const VulkanRenderPass& other) = delete;
  VulkanRenderPass& operator=(const VulkanRenderPass& other) = delete;

  VulkanRenderPass(VulkanRenderPass&& other) noexcept;
  VulkanRenderPass& operator=(VulkanRenderPass&& other) noexcept;

  VkRenderPass getRawRenderPass() { return renderPass_; }

 private:
  VulkanGraphicsDevice* device_;
  VkRenderPass renderPass_;
};

} // namespace blocks::render
