#pragma once

#include "engine/ColorRenderableResource.hpp"
#include "engine/FontResource.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class MainMenuUI;

class MainMenuUIResourceSentinel {
 public:
  static void load();
  static void unload();
};

struct MainMenuUIPrototype {
  engine::ResourceRef<ColorRenderableResource> colorResource;
  engine::ResourceRef<FontResource> fontResource;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"colorResource">,
          engine::ResourceRef<ColorRenderableResource>>,
      util::TPair<
          util::TString<"fontResource">,
          engine::ResourceRef<FontResource>>>;
};

struct MainMenuUIDefinition {
  engine::ResourceRef<MainMenuUIPrototype> prototype;

  using Fields = util::TArray<util::TPair<
      util::TString<"prototype">,
      engine::ResourceRef<MainMenuUIPrototype>>>;
  using ActorType = MainMenuUI;
};

class MainMenuUI : public UIActor {
 public:
  MainMenuUI(Scene& scene, const MainMenuUIDefinition& definition);
  MainMenuUI(Scene& scene);
};

} // namespace blocks::game
