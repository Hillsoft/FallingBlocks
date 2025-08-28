#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"

namespace blocks::game {

class BlockResourceSentinel {
 public:
  static void load();
  static void unload();
};

class Block : public Actor, public physics::RectCollider, public Drawable {
 public:
  Block(
      Scene& scene,
      math::Vec2 p0,
      math::Vec2 p1,
      render::RenderableRef<render::RenderableTex2D::InstanceData> sprite);
  Block(Scene& scene, math::Vec2 p0, math::Vec2 p1);

  void draw() final;

  void onDestroy() override;

 private:
  render::RenderableRef<render::RenderableTex2D::InstanceData> sprite_;
};

} // namespace blocks::game
