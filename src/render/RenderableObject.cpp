#include "render/RenderableObject.hpp"

#include <utility>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanDescriptorPool.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

RenderableObject::RenderableObject(
    VulkanShaderProgram* shaderProgram,
    VulkanDescriptorPool descriptorPool,
    VulkanBuffer vertexAttributes,
    size_t instanceDataSize)
    : shaderProgram_(shaderProgram),
      descriptorPool_(std::move(descriptorPool)),
      vertexAttributes_(std::move(vertexAttributes)),
      instanceDataSize_(instanceDataSize) {}

} // namespace blocks::render
