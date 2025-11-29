#include "render/shaders/UVVertex.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace blocks::render {

void UVVertex::appendVertexAttributeDescriptors(
    uint32_t binding,
    std::vector<VkVertexInputAttributeDescription>& descriptor,
    uint32_t& locationOffset) {
  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset,
          .binding = binding,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(UVVertex, position)});

  descriptor.emplace_back(
      VkVertexInputAttributeDescription{
          .location = locationOffset + 1,
          .binding = binding,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(UVVertex, uv)});

  locationOffset += 2;
}

} // namespace blocks::render
