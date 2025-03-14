#pragma once

#include <filesystem>
#include <optional>
#include <GLFW/glfw3.h>
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanImageView.hpp"
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render {

class VulkanTexture {
 public:
  VulkanTexture(
      VulkanGraphicsDevice& device,
      VulkanCommandPool& commandPool,
      const std::filesystem::path& source);

  VulkanImageView& getImageView() { return *imageView_; }
  VkSampler getSampler() { return sampler_.get(); }

 private:
  vulkan::UniqueHandle<VkDeviceMemory> deviceMemory_;
  vulkan::UniqueHandle<VkBuffer> buffer_;
  vulkan::UniqueHandle<VkImage> image_;
  std::optional<VulkanImageView> imageView_;
  vulkan::UniqueHandle<VkSampler> sampler_;
};

} // namespace blocks::render
