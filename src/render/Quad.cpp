#include "render/Quad.hpp"

#include <cstddef>
#include <span>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/shaders/UVVertex.hpp"

namespace blocks::render {

namespace {

std::vector<UVVertex> vertices = {
    {{0.0f, 1.0f}, {0.0f, 1.0f}},
    {{0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.0f, 1.0f}, {0.0f, 1.0f}},
    {{1.0f, 0.0f}, {1.0f, 0.0f}}};

} // namespace

VulkanBuffer getQuadVertexAttributesBuffer(VulkanGraphicsDevice& device) {
  return VulkanBuffer{
      device,
      std::span{
          reinterpret_cast<std::byte*>(vertices.data()),
          sizeof(UVVertex) * vertices.size()},
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
}

} // namespace blocks::render
