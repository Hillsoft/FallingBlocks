#pragma once

#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

VulkanBuffer getQuadVertexAttributesBuffer(VulkanGraphicsDevice& device);

} // namespace blocks::render
