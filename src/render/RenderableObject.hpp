#pragma once

#include <cstdint>
#include <filesystem>
#include <utility>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"
#include "render/VulkanTexture.hpp"
#include "render/resource/ShaderProgramManager.hpp"
#include "render/resource/TextureManager.hpp"

namespace blocks::render {

class RenderSubSystem;

class RenderableObject {
 public:
  RenderableObject(
      const std::filesystem::path& texturePath,
      VulkanGraphicsDevice& device,
      ShaderProgramManager& programManager,
      TextureManager& textureManager,
      uint32_t maxFramesInFlight);
  RenderableObject(
      VulkanShaderProgram* shaderProgram,
      VulkanDescriptorPool descriptorPool,
      VulkanBuffer vertexAttributes,
      VulkanTexture* texture,
      size_t instanceDataSize)
      : shaderProgram_(shaderProgram),
        descriptorPool_(std::move(descriptorPool)),
        vertexAttributes_(std::move(vertexAttributes)),
        texture_(texture),
        instanceDataSize_(instanceDataSize) {}

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
