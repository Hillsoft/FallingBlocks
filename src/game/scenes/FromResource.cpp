#include "game/scenes/FromResource.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "GlobalSubSystemStack.hpp"
#include "ResourceTypes.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/BlocksScene.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game::scene {

struct SceneObjectDefinition {
  std::string type;

  using Fields = util::TArray<util::TPair<util::TString<"type">, std::string>>;
};

struct SceneDefition {
  SceneObjectDefinition sceneObjectDefinition;
  std::vector<GameObjects> objects;

  using Fields = util::TArray<
      util::TPair<util::TString<"sceneObject">, SceneObjectDefinition>,
      util::TPair<util::TString<"actors">, std::vector<GameObjects>>>;
};

struct SceneDefinitionWrapper {
  std::string objectType;
  SceneDefition definition;

  using Fields = util::TArray<
      util::TPair<util::TString<"objectType">, std::string>,
      util::TPair<util::TString<"data">, SceneDefition>>;
};

std::unique_ptr<Scene> fromResourceName(std::string sceneName) {
  blocks::engine::ResourceRef<SceneDefinitionWrapper> sceneDefinitionRef =
      GlobalSubSystemStack::get()
          .resourceManager()
          .loadResource<SceneDefinitionWrapper>(std::move(sceneName));

  // TODO: use the object type we were actually asked to create
  std::unique_ptr<Scene> scene = std::make_unique<game::BlocksScene>();

  for (auto& actor : sceneDefinitionRef->definition.objects) {
    actor.visit([&](auto actorDefinition) {
      scene->createActor<decltype(actorDefinition)::ActorType>(actorDefinition);
    });
  }

  return scene;
}

} // namespace blocks::game::scene
