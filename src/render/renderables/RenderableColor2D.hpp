#pragma once

#include "render/RenderableObject.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"
#include "render/shaders/Col2DShader.hpp"

namespace blocks::render {

class RenderableColor2D {
 public:
  RenderableColor2D() = delete;

  using InstanceData = Col2DShader::InstanceData;

  static RenderableObject create(
      VulkanGraphicsDevice& device,
      ShaderProgramManager& programManager,
      TextureManager& textureManager,
      int maxFramesInFlight);
};

} // namespace blocks::render
