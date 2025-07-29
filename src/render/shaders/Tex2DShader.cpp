#include "render/shaders/Tex2DShader.hpp"

#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanShaderProgram.hpp"
#include "render/vulkan/DescriptorSetLayoutBuilder.hpp"

namespace blocks::render {

VulkanShaderProgram Tex2DShader::makeProgram(
    VulkanGraphicsDevice& device, VkRenderPass renderPass) {
  return {
      device,
      renderPass,
      getQuadVertexShader(device),
      VulkanShader{device, "shaders/fragment.spv"},
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
          .build(device.getRawDevice())};
}

} // namespace blocks::render
