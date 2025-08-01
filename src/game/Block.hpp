#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/RenderableQuad.hpp"

namespace blocks::game {

class Block : public Actor, public physics::RectCollider, public Drawable {
 public:
  Block(Scene& scene, math::Vec2 p0, math::Vec2 p1);

  void draw() final;

 private:
  render::UniqueRenderableHandle<render::UniformData> sprite_;
};

} // namespace blocks::game
