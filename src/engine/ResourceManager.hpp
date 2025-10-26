#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "engine/ResourceRef.hpp"
#include "serialization/Serialization.hpp"
#include "serialization/yaml/YAMLSerializationProvider.hpp"
#include "util/TypeErasedUniquePtr.hpp"
#include "util/file.hpp"
#include "util/string.hpp"

namespace blocks::engine {

class ResourceManager {
 public:
  static ResourceManager& get();

  template <typename T>
  ResourceRef<T> loadResource(std::string resourceName) {
    auto it = resources_.find(resourceName);
    if (it != resources_.end()) {
      const auto& resourcePtr = it->second;
      if (!resourcePtr.holdsType<T>()) {
        throw std::runtime_error{
            util::toString("Resource has unexpected type: ", resourceName)};
      }
      return ResourceRef<T>{resourcePtr.get<T>()};
    }

    std::vector<char> fileContents = util::readFileChars(
        (std::filesystem::path("data") / resourceName)
            .replace_extension("yaml"));
    T resource = serialization::
        deserialize<T, serialization::yaml::YAMLSerializationProvider>(
            {fileContents.begin(), fileContents.end()});

    const auto& inserted = resources_.emplace(
        std::move(resourceName),
        util::TypeErasedUniquePtr{std::make_unique<T>(std::move(resource))});
    return ResourceRef<T>{inserted.first->second.get<T>()};
  }

 private:
  std::unordered_map<std::string, util::TypeErasedUniquePtr> resources_;
};

} // namespace blocks::engine

namespace blocks::serialization {
template <typename T>
struct deserializeArbitrary<engine::ResourceRef<T>> {
  template <typename TCursor>
  engine::ResourceRef<T> operator()(TCursor cursor) {
    return engine::ResourceManager::get().loadResource<T>(
        std::string{cursor.getStringValue()});
  }
};
} // namespace blocks::serialization
