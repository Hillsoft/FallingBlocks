#pragma once

#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "ui/UIText.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class ScoreUI;

struct ScoreUIDefinition {
  using Fields = util::TArray<>;
  using ActorType = ScoreUI;
};

class ScoreUI : public UIActor {
 public:
  ScoreUI(Scene& scene, const ScoreUIDefinition& definition);
  ScoreUI(Scene& scene);

  void draw() final;

 protected:
  ui::UIText* scoreElement_;
};

} // namespace blocks::game
