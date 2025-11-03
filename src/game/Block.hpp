#pragma once

#include <optional>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TextureResource.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class Block;

struct BlockPrototype {
  engine::ResourceRef<engine::TextureResource> texture;
  math::Vec2 size;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"texture">,
          engine::ResourceRef<engine::TextureResource>>,
      util::TPair<util::TString<"size">, math::Vec2>>;
};

struct BlockDefinition {
  engine::ResourceRef<BlockPrototype> prototype;
  math::Vec2 position;
  std::optional<math::Vec2> size;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"prototype">,
          engine::ResourceRef<BlockPrototype>>,
      util::TPair<util::TString<"position">, math::Vec2>,
      util::TPair<util::TString<"size">, std::optional<math::Vec2>>>;
  using ActorType = Block;
};

class Block : public Actor, public physics::RectCollider, public Drawable {
 public:
  Block(Scene& scene, const BlockDefinition& definition);
  Block(
      Scene& scene,
      math::Vec2 p0,
      math::Vec2 p1,
      engine::ResourceRef<BlockPrototype> prototype);

  void draw() final;

  void onDestroy() override;

 private:
  engine::ResourceRef<BlockPrototype> prototype_;
};

} // namespace blocks::game
