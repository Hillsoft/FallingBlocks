#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanDescriptorPool {
 public:
  VulkanDescriptorPool(
      VulkanGraphicsDevice& device,
      VkDescriptorSetLayout layout,
      int maxFramesInFlight);

  [[nodiscard]] const std::vector<VkDescriptorSet>& getDescriptorSets() const {
    return descriptorSets_;
  }

 private:
  vulkan::UniqueHandle<VkDescriptorPool> pool_;
  std::vector<VkDescriptorSet> descriptorSets_;
};

} // namespace blocks::render
