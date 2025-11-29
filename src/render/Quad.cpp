#include "render/Quad.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <vulkan/vulkan_core.h>
#include "math/vec.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/shaders/UVVertex.hpp"

namespace blocks::render {

namespace {

constexpr std::array<UVVertex, 6> vertices{
    UVVertex{.position = math::Vec2{0.0f, 1.0f}, .uv = math::Vec2{0.0f, 1.0f}},
    UVVertex{.position = math::Vec2{0.0f, 0.0f}, .uv = math::Vec2{0.0f, 0.0f}},
    UVVertex{.position = math::Vec2{1.0f, 0.0f}, .uv = math::Vec2{1.0f, 0.0f}},
    UVVertex{.position = math::Vec2{1.0f, 1.0f}, .uv = math::Vec2{1.0f, 1.0f}},
    UVVertex{.position = math::Vec2{0.0f, 1.0f}, .uv = math::Vec2{0.0f, 1.0f}},
    UVVertex{.position = math::Vec2{1.0f, 0.0f}, .uv = math::Vec2{1.0f, 0.0f}}};

} // namespace

VulkanBuffer getQuadVertexAttributesBuffer(VulkanGraphicsDevice& device) {
  return VulkanBuffer{
      device,
      std::span{
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          reinterpret_cast<const std::byte*>(vertices.data()),
          sizeof(UVVertex) * vertices.size()},
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
}

} // namespace blocks::render
