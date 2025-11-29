#pragma once

#include <filesystem>
#include <vulkan/vulkan_core.h>
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanTexture {
 public:
  VulkanTexture(
      VulkanGraphicsDevice& device,
      VulkanCommandPool& commandPool,
      const std::filesystem::path& source);

  VkImageView getImageView() { return imageView_.get(); }
  VkSampler getSampler() { return sampler_.get(); }

 private:
  vulkan::UniqueHandle<VkDeviceMemory> deviceMemory_;
  vulkan::UniqueHandle<VkBuffer> buffer_;
  vulkan::UniqueHandle<VkImage> image_;
  vulkan::UniqueHandle<VkImageView> imageView_;
  vulkan::UniqueHandle<VkSampler> sampler_;
};

} // namespace blocks::render
