#include "engine/SceneLoader.hpp"

#include <memory>
#include <string>
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
  blocks::engine::ResourceRef<SceneDefinition> sceneDefinitionRef =
      GlobalSubSystemStack::get()
          .resourceManager()
          .loadResource<SceneDefinition>(std::move(sceneName));

  return loadSceneFromDefinition(sceneDefinitionRef);
}

std::unique_ptr<Scene> loadSceneFromDefinition(
    engine::ResourceRef<SceneDefinition> sceneDefinitionRef) {
  std::unique_ptr<Scene> scene = sceneDefinitionRef->sceneObject.visit(
      [&](auto sceneDefinition) -> std::unique_ptr<Scene> {
        return std::make_unique<typename decltype(sceneDefinition)::SceneType>(
            sceneDefinition);
      });

  for (auto& actor : sceneDefinitionRef->objects) {
    actor.visit([&](auto actorDefinition) {
      scene->createActor<typename decltype(actorDefinition)::ActorType>(
          actorDefinition);
    });
  }

  return scene;
}

} // namespace blocks
