#pragma once

#include <filesystem>
#include <GLFW/glfw3.h>

#include "render/VulkanGraphicsDevice.hpp"

namespace tetris::render {

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

} // namespace tetris::render
