#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class LoadingScreen : public Actor, public Drawable, public TickHandler {
 public:
  LoadingScreen(Scene& scene);

  void update(float deltaTimeSeconds) final;
  void draw() final;

 private:
  render::UniqueRenderableHandle image1_;
  render::UniqueRenderableHandle image2_;
  float toTransition = 0.f;
  bool firstActive_ = true;
};

} // namespace blocks::game
