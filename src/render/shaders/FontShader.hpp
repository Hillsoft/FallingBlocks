#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include "math/vec.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

class FontShader {
 public:
  FontShader() = delete;

  struct InstanceData {
    math::Mat3 modelMatrix;
    int32_t glyphStart;
    int32_t glyphEnd;
    math::Mat3 uvToGlyphSpace;
  };

  static VulkanShaderProgram makeProgram(
      VulkanGraphicsDevice& device, VkRenderPass renderPass);
};

} // namespace blocks::render
