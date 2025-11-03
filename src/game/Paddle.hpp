#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TextureResource.hpp"
#include "engine/TickRegistry.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class Paddle;

struct PaddlePrototype {
  engine::ResourceRef<engine::TextureResource> texture;

  using Fields = util::TArray<util::TPair<
      util::TString<"texture">,
      engine::ResourceRef<engine::TextureResource>>>;
};

struct PaddleDefinition {
  engine::ResourceRef<PaddlePrototype> prototype;

  using Fields = util::TArray<util::TPair<
      util::TString<"prototype">,
      engine::ResourceRef<PaddlePrototype>>>;
  using ActorType = Paddle;
};

class Paddle
    : public Actor,
      public input::InputHandler,
      public physics::RectCollider,
      public TickHandler,
      public Drawable {
 public:
  Paddle(Scene& scene, const PaddleDefinition& definition);

  void update(float deltaTimeSeconds) final;
  void draw() final;

  void onKeyPress(int key) final;
  void onKeyRelease(int key) final;

 private:
  engine::ResourceRef<PaddlePrototype> prototype_;
  math::Vec2 vel_;
};

} // namespace blocks::game
