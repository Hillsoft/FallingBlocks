#pragma once

#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShader.hpp"

namespace blocks::render {

VulkanVertexShader getQuadVertexShader(VulkanGraphicsDevice& device);
VulkanBuffer getQuadVertexAttributesBuffer(VulkanGraphicsDevice& device);

} // namespace blocks::render
