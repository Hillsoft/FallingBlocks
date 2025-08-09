#pragma once

#include <filesystem>
#include "math/vec.hpp"
#include "render/RenderableObject.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"

namespace blocks::render {

class RenderableTex2D {
 public:
  RenderableTex2D() = delete;

  struct InstanceData {
    math::Mat3 modelMatrix;
  };

  static RenderableObject create(
      const std::filesystem::path& texturePath,
      VulkanGraphicsDevice& device,
      ShaderProgramManager& programManager,
      TextureManager& textureManager,
      int maxFramesInFlight);
};

} // namespace blocks::render
