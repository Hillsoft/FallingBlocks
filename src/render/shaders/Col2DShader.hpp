#pragma once

#include <vulkan/vulkan_core.h>
#include "math/vec.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

class Col2DShader {
 public:
  Col2DShader() = delete;

  struct InstanceData {
    math::Mat3 modelMatrix;
    math::Vec4 color;
  };

  static VulkanShaderProgram makeProgram(
      VulkanGraphicsDevice& device, VkRenderPass renderPass);
};

} // namespace blocks::render
