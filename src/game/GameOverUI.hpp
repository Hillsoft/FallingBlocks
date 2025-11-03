#pragma once

#include "engine/FontResource.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "input/InputHandler.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class GameOverUI;

struct GameOverUIPrototype {
  engine::ResourceRef<FontResource> fontResource;

  using Fields = util::TArray<util::TPair<
      util::TString<"fontResource">,
      engine::ResourceRef<FontResource>>>;
};

struct GameOverUIDefinition {
  engine::ResourceRef<GameOverUIPrototype> prototype;

  using Fields = util::TArray<util::TPair<
      util::TString<"prototype">,
      engine::ResourceRef<GameOverUIPrototype>>>;
  using ActorType = GameOverUI;
};

class GameOverUI : public UIActor, public input::InputHandler {
 public:
  GameOverUI(Scene& scene, const GameOverUIDefinition& definition);

  void onKeyRelease(int keyCode) final;
};

} // namespace blocks::game
