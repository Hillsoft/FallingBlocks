#pragma once

#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "ui/UIText.hpp"

namespace blocks::game {

class ScoreUI : public UIActor {
 public:
  ScoreUI(Scene& scene);

  void draw() final;

 protected:
  ui::UIText* scoreElement_;
};

} // namespace blocks::game
