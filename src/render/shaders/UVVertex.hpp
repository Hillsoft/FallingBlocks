#pragma once

#include <cstdint>
#include <vector>
#include <GLFW/glfw3.h>
#include "math/vec.hpp"

namespace blocks::render {

struct UVVertex {
  math::Vec2 position;
  math::Vec2 uv;

  static void appendVertexAttributeDescriptors(
      uint32_t binding,
      std::vector<VkVertexInputAttributeDescription>& descriptor,
      uint32_t& locationOffset);
};

} // namespace blocks::render
