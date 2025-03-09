#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

class VulkanPipelineLayout {
 public:
  VulkanPipelineLayout(VulkanGraphicsDevice& device);
  ~VulkanPipelineLayout();

  VulkanPipelineLayout(const VulkanPipelineLayout& other) = delete;
  VulkanPipelineLayout& operator=(const VulkanPipelineLayout& other) = delete;

  VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept;
  VulkanPipelineLayout& operator=(VulkanPipelineLayout&& other) noexcept;

  VkPipelineLayout getRawLayout() { return layout_; }

 private:
  VulkanGraphicsDevice* device_;
  VkPipelineLayout layout_;
};

} // namespace blocks::render
