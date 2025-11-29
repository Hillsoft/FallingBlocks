#pragma once

#include <optional>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TextureResource.hpp"
#include "engine/TickRegistry.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class Ball;

struct BallPrototype {
  engine::ResourceRef<engine::TextureResource> texture;

  using Fields = util::TArray<util::TPair<
      util::TString<"texture">,
      engine::ResourceRef<engine::TextureResource>>>;
};

struct BallDefinition {
  engine::ResourceRef<BallPrototype> prototype{nullptr};
  std::optional<math::Vec2> position;
  std::optional<math::Vec2> velocity;

  using Fields = util::TArray<
      util::
          TPair<util::TString<"prototype">, engine::ResourceRef<BallPrototype>>,
      util::TPair<util::TString<"position">, std::optional<math::Vec2>>,
      util::TPair<util::TString<"velocity">, std::optional<math::Vec2>>>;
  using ActorType = Ball;
};

class Ball
    : public Actor,
      public physics::RectCollider,
      public TickHandler,
      public Drawable {
 public:
  Ball(Scene& scene, const BallDefinition& definition);
  Ball(
      Scene& scene,
      engine::ResourceRef<BallPrototype> prototype,
      math::Vec2 pos,
      math::Vec2 vel);

  void update(float deltaTimeSeconds) final;
  void draw() final;
  void onDestroy() final;

  void handleCollision(physics::RectCollider& other, math::Vec2 normal) final;

 private:
  engine::ResourceRef<BallPrototype> prototype_;
  math::Vec2 vel_;
};

} // namespace blocks::game
