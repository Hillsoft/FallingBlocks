#pragma once

#include <memory>
#include <string>
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"

namespace blocks {

struct SceneDefinition;

std::unique_ptr<Scene> loadSceneFromName(std::string sceneName);
std::unique_ptr<Scene> loadSceneFromDefinition(
    engine::ResourceRef<SceneDefinition> sceneDefinitionRef);

} // namespace blocks
