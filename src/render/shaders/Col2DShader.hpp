
#pragma once

#include <GLFW/glfw3.h>
#include "math/vec.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

class Col2DShader {
 public:
  Col2DShader() = delete;

  struct InstanceData {
    math::Vec2 pos0;
    math::Vec2 pos1;
    math::Vec4 color;
  };

  static VulkanShaderProgram makeProgram(
      VulkanGraphicsDevice& device, VkRenderPass renderPass);
};

} // namespace blocks::render
