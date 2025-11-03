#pragma once

#include <optional>
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TextureResource.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "math/vec.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class BallSpawnBlock;

struct BallSpawnBlockPrototype : public BlockPrototype {
  BallSpawnBlockPrototype(
      engine::ResourceRef<engine::TextureResource> texture,
      math::Vec2 size,
      engine::ResourceRef<BallPrototype> ballPrototype)
      : BlockPrototype(texture, size), ballPrototype(ballPrototype) {}

  engine::ResourceRef<BallPrototype> ballPrototype;

  using Fields = BlockPrototype::Fields::Append<util::TPair<
      util::TString<"ballPrototype">,
      engine::ResourceRef<BallPrototype>>>;
};

struct BallSpawnBlockDefinition : public BlockDefinition {
  BallSpawnBlockDefinition(
      engine::ResourceRef<BallSpawnBlockPrototype> prototype,
      math::Vec2 position,
      std::optional<math::Vec2> size)
      : BlockDefinition(prototype, position, size), prototype(prototype) {}

  engine::ResourceRef<BallSpawnBlockPrototype> prototype;

  using Fields = util::TArray<
      util::TPair<
          util::TString<"prototype">,
          engine::ResourceRef<BallSpawnBlockPrototype>>,
      util::TPair<util::TString<"position">, math::Vec2>,
      util::TPair<util::TString<"size">, std::optional<math::Vec2>>>;
  using ActorType = BallSpawnBlock;
};

class BallSpawnBlock : public Block {
 public:
  BallSpawnBlock(Scene& scene, const BallSpawnBlockDefinition& definition);

  void onDestroy() final;

 private:
  engine::ResourceRef<BallPrototype> ballPrototype_;
};

} // namespace blocks::game
