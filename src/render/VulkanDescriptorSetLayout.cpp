#include "render/VulkanDescriptorSetLayout.hpp"

#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/DescriptorSetLayoutBuilder.hpp"

namespace blocks::render {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
    VulkanGraphicsDevice& device)
    : descriptorSet_(
          vulkan::DescriptorSetLayoutBuilder()
              .addBinding(
                  0,
                  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                  1,
                  VK_SHADER_STAGE_FRAGMENT_BIT)
              .addBinding(
                  1,
                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                  1,
                  VK_SHADER_STAGE_VERTEX_BIT)
              .build(device.getRawDevice())) {}

} // namespace blocks::render
