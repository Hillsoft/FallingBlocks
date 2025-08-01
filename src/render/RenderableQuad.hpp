#pragma once

#include <cstdint>
#include <filesystem>
#include "math/vec.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"
#include "render/VulkanTexture.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"

namespace blocks::render {

class RenderSubSystem;

struct UniformData {
  math::Vec<float, 2> pos0;
  math::Vec<float, 2> pos1;
};

class RenderableQuad {
 public:
  RenderableQuad(
      const std::filesystem::path& texturePath,
      VulkanGraphicsDevice& device,
      ShaderProgramManager& programManager,
      TextureManager& textureManager,
      uint32_t maxFramesInFlight);

 public:
  size_t getInstanceDataSize() const { return instanceDataSize_; }

 private:
  VulkanShaderProgram* shaderProgram_;
  VulkanDescriptorPool descriptorPool_;
  VulkanBuffer vertexAttributes_;
  VulkanTexture* texture_;
  size_t instanceDataSize_;

 public:
  friend class RenderSubSystem;
};

} // namespace blocks::render
