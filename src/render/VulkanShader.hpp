#pragma once

#include <filesystem>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanShader {
 public:
  VulkanShader(
      VulkanGraphicsDevice& device, const std::filesystem::path& shaderPath);

  VkShaderModule getRawShader() { return shaderModule_.get(); }

 private:
  vulkan::UniqueHandle<VkShaderModule> shaderModule_;
};

class VulkanVertexShader {
 public:
  VulkanVertexShader(
      VulkanGraphicsDevice& device,
      std::vector<VkVertexInputBindingDescription> bindingDescriptions,
      std::vector<VkVertexInputAttributeDescription> inputAttributeDescription,
      const std::filesystem::path& shaderPath);

  [[nodiscard]] const std::vector<VkVertexInputBindingDescription>&
  getInputBindingDescriptions() const {
    return bindingDescriptions_;
  }

  [[nodiscard]] const std::vector<VkVertexInputAttributeDescription>&
  getInputAttributeDescriptions() const {
    return inputAttributeDescription_;
  }

  VkShaderModule getRawShader() { return shader_.getRawShader(); }

 private:
  VulkanShader shader_;
  std::vector<VkVertexInputBindingDescription> bindingDescriptions_;
  std::vector<VkVertexInputAttributeDescription> inputAttributeDescription_;
};

} // namespace blocks::render
