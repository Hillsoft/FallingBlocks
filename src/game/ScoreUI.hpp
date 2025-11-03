#pragma once

#include "engine/FontResource.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "ui/UIText.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class ScoreUI;

struct ScoreUIPrototype {
  engine::ResourceRef<FontResource> font;

  using Fields = util::TArray<
      util::TPair<util::TString<"font">, engine::ResourceRef<FontResource>>>;
};

struct ScoreUIDefinition {
  engine::ResourceRef<ScoreUIPrototype> prototype;

  using Fields = util::TArray<util::TPair<
      util::TString<"prototype">,
      engine::ResourceRef<ScoreUIPrototype>>>;
  using ActorType = ScoreUI;
};

class ScoreUI : public UIActor {
 public:
  ScoreUI(Scene& scene, const ScoreUIDefinition& definition);

  void draw() final;

 protected:
  ui::UIText* scoreElement_;
};

} // namespace blocks::game
