#pragma once

#include <filesystem>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

class VulkanShader {
 public:
  VulkanShader(
      VulkanGraphicsDevice& device, const std::filesystem::path& shaderPath);
  ~VulkanShader();

  VulkanShader(const VulkanShader& other) = delete;
  VulkanShader& operator=(const VulkanShader& other) = delete;

  VulkanShader(VulkanShader&& other) noexcept;
  VulkanShader& operator=(VulkanShader&& other) noexcept;

  VkShaderModule getRawShader() { return shaderModule_; }

 private:
  VulkanGraphicsDevice* device_;
  VkShaderModule shaderModule_;
};

class VulkanVertexShader {
 public:
  VulkanVertexShader(
      VulkanGraphicsDevice& device,
      std::vector<VkVertexInputBindingDescription> bindingDescriptions,
      std::vector<VkVertexInputAttributeDescription> inputAttributeDescription,
      const std::filesystem::path& shaderPath);

  const std::vector<VkVertexInputBindingDescription>&
  getInputBindingDescriptions() const {
    return bindingDescriptions_;
  }

  const std::vector<VkVertexInputAttributeDescription>&
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
