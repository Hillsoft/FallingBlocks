#pragma once

#include "engine/FontResource.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "input/InputHandler.hpp"

namespace blocks::game {

class GameOverUI;

class GameOverUIResourceSentinel {
 public:
  static void load();
  static void unload();
};

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
