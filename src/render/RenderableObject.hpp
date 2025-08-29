#pragma once

#include <memory>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

class RenderSubSystem;

class RenderableObject {
 public:
  class ResourceHolder {
   public:
    virtual ~ResourceHolder() = default;
  };

  RenderableObject(
      VulkanShaderProgram* shaderProgram,
      VulkanDescriptorPool descriptorPool,
      VulkanBuffer vertexAttributes,
      size_t instanceDataSize,
      std::unique_ptr<ResourceHolder> extraResources = nullptr);

 public:
  size_t getInstanceDataSize() const { return instanceDataSize_; }

 private:
  VulkanShaderProgram* shaderProgram_;
  VulkanDescriptorPool descriptorPool_;
  VulkanBuffer vertexAttributes_;
  size_t instanceDataSize_;
  std::unique_ptr<ResourceHolder> extraResources_;

 public:
  friend class RenderSubSystem;
};

} // namespace blocks::render
