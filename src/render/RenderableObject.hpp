#pragma once

#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

class RenderSubSystem;

class RenderableObject {
 public:
  RenderableObject(
      VulkanShaderProgram* shaderProgram,
      VulkanDescriptorPool descriptorPool,
      VulkanBuffer vertexAttributes,
      size_t instanceDataSize);

 public:
  size_t getInstanceDataSize() const { return instanceDataSize_; }

 private:
  VulkanShaderProgram* shaderProgram_;
  VulkanDescriptorPool descriptorPool_;
  VulkanBuffer vertexAttributes_;
  size_t instanceDataSize_;

 public:
  friend class RenderSubSystem;
};

} // namespace blocks::render
