#pragma once

#include <cstdint>
#include <vector>
#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class DescriptorSetLayoutBuilder {
 public:
  explicit DescriptorSetLayoutBuilder();

  DescriptorSetLayoutBuilder& addBinding(
      uint32_t binding,
      VkDescriptorType descriptorType,
      uint32_t descriptorCount,
      VkShaderStageFlags stageFlags);

  UniqueHandle<VkDescriptorSetLayout> build(VkDevice device);

 private:
  std::vector<VkDescriptorSetLayoutBinding> bindings_;
};

} // namespace blocks::render::vulkan
