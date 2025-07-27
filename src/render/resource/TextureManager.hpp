#pragma once

#include <filesystem>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/VulkanTexture.hpp"

namespace blocks::render {

class TextureManager {
 public:
  TextureManager(VulkanGraphicsDevice& device, VulkanCommandPool commandPool)
      : device_(&device), commandPool_(std::move(commandPool)) {}

  VulkanTexture& getOrCreate(const std::filesystem::path& resourceLocation);

 private:
  std::unordered_map<std::string, VulkanTexture> textures_;
  VulkanGraphicsDevice* device_;
  VulkanCommandPool commandPool_;
};

} // namespace blocks::render
