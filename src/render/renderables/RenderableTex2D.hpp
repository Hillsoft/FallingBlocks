#pragma once

#include <filesystem>
#include "render/RenderableObject.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"
#include "render/shaders/Tex2DShader.hpp"

namespace blocks::render {

class RenderableTex2D {
 public:
  RenderableTex2D() = delete;

  using InstanceData = typename Tex2DShader::InstanceData;

  static RenderableObject create(
      const std::filesystem::path& texturePath,
      VulkanGraphicsDevice& device,
      ShaderProgramManager& programManager,
      TextureManager& textureManager,
      int maxFramesInFlight);
};

} // namespace blocks::render
