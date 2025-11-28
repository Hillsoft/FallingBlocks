#include "render/Quad.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <GLFW/glfw3.h>
#include "math/vec.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/shaders/UVVertex.hpp"

namespace blocks::render {

namespace {

constexpr std::array<UVVertex, 6> vertices{
    UVVertex{math::Vec2{0.0f, 1.0f}, math::Vec2{0.0f, 1.0f}},
    UVVertex{math::Vec2{0.0f, 0.0f}, math::Vec2{0.0f, 0.0f}},
    UVVertex{math::Vec2{1.0f, 0.0f}, math::Vec2{1.0f, 0.0f}},
    UVVertex{math::Vec2{1.0f, 1.0f}, math::Vec2{1.0f, 1.0f}},
    UVVertex{math::Vec2{0.0f, 1.0f}, math::Vec2{0.0f, 1.0f}},
    UVVertex{math::Vec2{1.0f, 0.0f}, math::Vec2{1.0f, 0.0f}}};

} // namespace

VulkanBuffer getQuadVertexAttributesBuffer(VulkanGraphicsDevice& device) {
  return VulkanBuffer{
      device,
      std::span{
          reinterpret_cast<const std::byte*>(vertices.data()),
          sizeof(UVVertex) * vertices.size()},
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
}

} // namespace blocks::render
