#include "render/RenderableObject.hpp"

#include <cstddef>
#include <memory>
#include <utility>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

RenderableObject::RenderableObject(
    VulkanShaderProgram* shaderProgram,
    VulkanDescriptorPool descriptorPool,
    VulkanBuffer vertexAttributes,
    size_t instanceDataSize,
    std::unique_ptr<ResourceHolder> extraResources)
    : shaderProgram_(shaderProgram),
      descriptorPool_(std::move(descriptorPool)),
      vertexAttributes_(std::move(vertexAttributes)),
      instanceDataSize_(instanceDataSize),
      extraResources_(std::move(extraResources)) {}

} // namespace blocks::render
