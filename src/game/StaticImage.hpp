#pragma once

#include <optional>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TextureResource.hpp"
#include "math/vec.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class StaticImage;

struct StaticImagePrototype {
  engine::ResourceRef<engine::TextureResource> texture;
  math::Vec2 size;
  int z;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"texture">,
          engine::ResourceRef<engine::TextureResource>>,
      util::TPair<util::TString<"size">, math::Vec2>,
      util::TPair<util::TString<"z">, int>>;
};

struct StaticImageDefinition {
  engine::ResourceRef<StaticImagePrototype> prototype;
  math::Vec2 position;
  std::optional<math::Vec2> size;
  std::optional<int> z;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"prototype">,
          engine::ResourceRef<StaticImagePrototype>>,
      util::TPair<util::TString<"position">, math::Vec2>,
      util::TPair<util::TString<"size">, std::optional<math::Vec2>>,
      util::TPair<util::TString<"z">, std::optional<int>>>;
  using ActorType = StaticImage;
};

class StaticImage : public Actor, public Drawable {
 public:
  StaticImage(Scene& scene, const StaticImageDefinition& definition);

  void draw() final;

 private:
  engine::ResourceRef<StaticImagePrototype> prototype_;
  math::Vec2 minPos_;
  math::Vec2 maxPos_;
  int z_;
};

} // namespace blocks::game
