#include "render/shaders/Tex2DShader.hpp"

#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShader.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

VulkanShaderProgram Tex2DShader::makeProgram(
    VulkanGraphicsDevice& device, VkRenderPass renderPass) {
  return {
      device,
      renderPass,
      getQuadVertexShader(device),
      VulkanShader{device, "shaders/fragment.spv"},
      VulkanDescriptorSetLayout{device}};
}

} // namespace blocks::render
