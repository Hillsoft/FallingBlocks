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

constexpr std::array<VkVertexInputBindingDescription, 2> kBindingDescription =
    []() {
      std::array<VkVertexInputBindingDescription, 2> bindingDescriptions{};

      bindingDescriptions[0].binding = 0;
      bindingDescriptions[0].stride = sizeof(VertexData);
      bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      bindingDescriptions[1].binding = 1;
      bindingDescriptions[1].stride = sizeof(float) * 9;
      bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

      return bindingDescriptions;
    }();

constexpr std::array<VkVertexInputAttributeDescription, 5>
    kInputAttributeDescriptions = []() {
      std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(VertexData, position);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(VertexData, uv);

      attributeDescriptions[2].binding = 1;
      attributeDescriptions[2].location = 2;
      attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[2].offset = 0;

      attributeDescriptions[3].binding = 1;
      attributeDescriptions[3].location = 3;
      attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[3].offset = sizeof(float) * 3;

      attributeDescriptions[4].binding = 1;
      attributeDescriptions[4].location = 4;
      attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[4].offset = sizeof(float) * 6;

      return attributeDescriptions;
    }();

} // namespace

VulkanVertexShader getQuadVertexShader(VulkanGraphicsDevice& device) {
  return VulkanVertexShader{
      device,
      {kBindingDescription.begin(), kBindingDescription.end()},
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
