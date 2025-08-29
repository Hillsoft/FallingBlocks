#pragma once

#include "render/RenderableObject.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"
#include "render/shaders/FontShader.hpp"

namespace blocks::render {

class RenderableFont {
 public:
  RenderableFont() = delete;

  using InstanceData = FontShader::InstanceData;

  static RenderableObject create(
      VulkanBuffer fontBuffer,
      VulkanGraphicsDevice& device,
      ShaderProgramManager& programManager,
      TextureManager& textureManager,
      int maxFramesInFlight);
};

} // namespace blocks::render
