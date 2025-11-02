#pragma once

#include <memory>
#include <string>
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"

namespace blocks {

struct SceneDefinitionWrapper;

class SceneLoader {
 public:
  virtual ~SceneLoader() = default;

  virtual std::unique_ptr<Scene> loadScene() const = 0;
};

class SceneLoaderFromResourceFile : public SceneLoader {
 public:
  SceneLoaderFromResourceFile(std::string sceneName);

  std::unique_ptr<Scene> loadScene() const final;

 private:
  std::string sceneName_;
};

std::unique_ptr<Scene> loadSceneFromDefinition(
    engine::ResourceRef<SceneDefinitionWrapper> sceneDefinitionRef);

} // namespace blocks
