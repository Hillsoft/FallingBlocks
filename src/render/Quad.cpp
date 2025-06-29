#include "render/Quad.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <vector>
#include <GLFW/glfw3.h>

#include "math/vec.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShader.hpp"

namespace blocks::render {

namespace {

struct VertexData {
  math::Vec2 position;
  math::Vec2 uv;
};

std::vector<VertexData> vertices = {
    {{0.0f, 1.0f}, {0.0f, 1.0f}},
    {{0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.0f, 1.0f}, {0.0f, 1.0f}},
    {{1.0f, 0.0f}, {1.0f, 0.0f}}};

constexpr VkVertexInputBindingDescription kBindingDescription = []() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(VertexData);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}();

constexpr std::array<VkVertexInputAttributeDescription, 2>
    kInputAttributeDescriptions = []() {
      std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(VertexData, position);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(VertexData, uv);

      return attributeDescriptions;
    }();

} // namespace

VulkanVertexShader getQuadVertexShader(VulkanGraphicsDevice& device) {
  return VulkanVertexShader{
      device,
      {kBindingDescription},
      {kInputAttributeDescriptions.begin(), kInputAttributeDescriptions.end()},
      "shaders/vertex.spv"};
}

VulkanBuffer getQuadVertexAttributesBuffer(VulkanGraphicsDevice& device) {
  return VulkanBuffer{
      device,
      std::span{
          reinterpret_cast<std::byte*>(vertices.data()),
          sizeof(VertexData) * vertices.size()},
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
}

} // namespace blocks::render
