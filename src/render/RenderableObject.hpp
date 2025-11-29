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
    ResourceHolder() = default;
    virtual ~ResourceHolder() = default;

    ResourceHolder(const ResourceHolder& other) = default;
    ResourceHolder& operator=(const ResourceHolder& other) = default;

    ResourceHolder(ResourceHolder&& other) = default;
    ResourceHolder& operator=(ResourceHolder&& other) = default;
  };

  RenderableObject(
      VulkanShaderProgram* shaderProgram,
      VulkanDescriptorPool descriptorPool,
      VulkanBuffer vertexAttributes,
      size_t instanceDataSize,
      std::unique_ptr<ResourceHolder> extraResources = nullptr);

  [[nodiscard]] size_t getInstanceDataSize() const { return instanceDataSize_; }

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
