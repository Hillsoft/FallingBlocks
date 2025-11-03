#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TextureResource.hpp"
#include "engine/TickRegistry.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class LoadingScreen;

struct LoadingScreenPrototype {
  engine::ResourceRef<engine::TextureResource> texture1;
  engine::ResourceRef<engine::TextureResource> texture2;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"texture1">,
          engine::ResourceRef<engine::TextureResource>>,
      util::TPair<
          util::TString<"texture2">,
          engine::ResourceRef<engine::TextureResource>>>;
};

struct LoadingScreenDefinition {
  engine::ResourceRef<LoadingScreenPrototype> prototype;

  using Fields = util::TArray<util::TPair<
      util::TString<"prototype">,
      engine::ResourceRef<LoadingScreenPrototype>>>;
  using ActorType = LoadingScreen;
};

class LoadingScreen : public Actor, public Drawable, public TickHandler {
 public:
  LoadingScreen(Scene& scene, const LoadingScreenDefinition& definition);

  void update(float deltaTimeSeconds) final;
  void draw() final;

 private:
  engine::ResourceRef<LoadingScreenPrototype> prototype_;
  float toTransition = 0.f;
  bool firstActive_ = true;
};

} // namespace blocks::game
