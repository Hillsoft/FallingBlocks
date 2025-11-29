#pragma once

#include <concepts>
#include <typeindex>
#include <unordered_map>
#include <vulkan/vulkan_core.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanShaderProgram.hpp"

namespace blocks::render {

template <typename T>
concept ShaderProgramDefinition = requires(
    VulkanGraphicsDevice& device, VkRenderPass renderPass) {
  { T::makeProgram(device, renderPass) } -> std::same_as<VulkanShaderProgram>;
};

class ShaderProgramManager {
 public:
  ShaderProgramManager(VulkanGraphicsDevice& device, VkRenderPass renderPass)
      : device_(&device), renderPass_(renderPass) {}

  template <ShaderProgramDefinition Shader>
  VulkanShaderProgram& getOrCreate() {
    const std::type_index index{typeid(Shader)};
    VulkanShaderProgram* program = get(index);
    if (program != nullptr) {
      return *program;
    }
    return insert(index, Shader::makeProgram(*device_, renderPass_));
  }

 private:
  VulkanShaderProgram* get(std::type_index index);
  VulkanShaderProgram& insert(
      std::type_index index, VulkanShaderProgram&& program);

  std::unordered_map<std::type_index, VulkanShaderProgram> programs_;
  VulkanGraphicsDevice* device_;
  VkRenderPass renderPass_;
};

} // namespace blocks::render
