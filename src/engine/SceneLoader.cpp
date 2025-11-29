#include "engine/SceneLoader.hpp"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "GlobalSubSystemStack.hpp"
#include "ResourceTypes.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "util/meta_utils.hpp"

namespace blocks {

struct SceneDefinition {
  SceneObjects sceneObject;
  std::vector<GameObjects> objects;

  using Fields = util::TArray<
      util::TPair<util::TString<"sceneObject">, SceneObjects>,
      util::TPair<util::TString<"actors">, std::vector<GameObjects>>>;
};

std::unique_ptr<Scene> loadSceneFromName(std::string sceneName) {
  return loadSceneFromDefinition(
      loadSceneDefinitionFromName(std::move(sceneName)));
}

engine::ResourceRef<SceneDefinition> loadSceneDefinitionFromName(
    std::string sceneName) {
  return GlobalSubSystemStack::get()
      .resourceManager()
      .loadResource<SceneDefinition>(std::move(sceneName));
}

std::unique_ptr<Scene> loadSceneFromDefinition(
    engine::ResourceRef<SceneDefinition> sceneDefinitionRef) {
  std::unique_ptr<Scene> scene = sceneDefinitionRef->sceneObject.visit(
      [&](const auto& sceneDefinition) -> std::unique_ptr<Scene> {
        return std::make_unique<
            typename std::remove_cvref_t<decltype(sceneDefinition)>::SceneType>(
            sceneDefinition);
      });

  for (auto& actor : sceneDefinitionRef->objects) {
    actor.visit([&](const auto& actorDefinition) {
      scene->createActor<
          typename std::remove_cvref_t<decltype(actorDefinition)>::ActorType>(
          actorDefinition);
    });
  }

  return scene;
}

} // namespace blocks
