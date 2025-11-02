#pragma once

#include "engine/Actor.hpp"
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class CameraActor;

struct CameraActorDefinition {
  math::Vec2 centre;
  math::Vec2 extent;

  using Fields = util::TArray<
      util::TPair<util::TString<"centre">, math::Vec2>,
      util::TPair<util::TString<"extent">, math::Vec2>>;
  using ActorType = CameraActor;
};

class CameraActor : public Actor {
 public:
  CameraActor(Scene& scene, const CameraActorDefinition& definition);
  CameraActor(Scene& scene, render::Simple2DCamera camera);

 private:
  render::Simple2DCamera camera_;
};

} // namespace blocks::game
