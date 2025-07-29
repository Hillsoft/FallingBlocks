#pragma once

#include <type_traits>
#include <GLFW/glfw3.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanGraphicsPipeline.hpp"
#include "render/VulkanShader.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanShaderProgram {
 public:
  VulkanShaderProgram(
      VulkanGraphicsDevice& device,
      VkRenderPass renderPass,
      VulkanVertexShader vertexShader,
      VulkanShader fragmentShader,
      vulkan::UniqueHandle<VkDescriptorSetLayout> descriptorSetLayout)
      : vertexShader_(std::move(vertexShader)),
        fragmentShader_(std::move(fragmentShader)),
        descriptorSetLayout_(std::move(descriptorSetLayout)),
        pipeline_(
            device,
            VK_FORMAT_B8G8R8A8_SRGB,
            renderPass,
            vertexShader_,
            fragmentShader_,
            descriptorSetLayout_.get()) {}

  VkDescriptorSetLayout getDescriptorSetLayout() {
    return descriptorSetLayout_.get();
  }

 private:
  VulkanVertexShader vertexShader_;
  VulkanShader fragmentShader_;
  vulkan::UniqueHandle<VkDescriptorSetLayout> descriptorSetLayout_;
  VulkanGraphicsPipeline pipeline_;

  friend class RenderSubSystem;
};

} // namespace blocks::render
