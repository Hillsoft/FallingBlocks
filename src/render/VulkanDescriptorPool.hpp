#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include "render/VulkanDescriptorSetLayout.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanDescriptorPool {
 public:
  VulkanDescriptorPool(
      VulkanGraphicsDevice& device,
      VulkanDescriptorSetLayout& layout,
      int maxFramesInFlight);

  const std::vector<VkDescriptorSet>& getDescriptorSets() const {
    return descriptorSets_;
  }

 private:
  vulkan::UniqueHandle<VkDescriptorPool> pool_;
  std::vector<VkDescriptorSet> descriptorSets_;
};

} // namespace blocks::render
