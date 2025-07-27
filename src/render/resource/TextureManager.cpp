#include "render/resource/TextureManager.hpp"

#include <filesystem>
#include <string>
#include <type_traits>
#include "render/VulkanTexture.hpp"

namespace blocks::render {

VulkanTexture& TextureManager::getOrCreate(
    const std::filesystem::path& resourceLocation) {
  std::string locationString = resourceLocation.generic_string();

  auto it = textures_.find(locationString);

  if (it != textures_.end()) {
    return it->second;
  }

  auto insertResult = textures_.emplace(
      std::move(locationString),
      VulkanTexture{*device_, commandPool_, resourceLocation});
  return insertResult.first->second;
}

} // namespace blocks::render
