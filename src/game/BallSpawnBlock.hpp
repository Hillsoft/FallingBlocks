#pragma once

#include "engine/Scene.hpp"
#include "game/Block.hpp"
#include "math/vec.hpp"

namespace blocks::game {

class BallSpawnBlockResourceSentinel {
 public:
  static void load();
  static void unload();
};

struct BallSpawnBlockDefinition : public BlockDefinition {
  using ActorType = Block;
};

class BallSpawnBlock : public Block {
 public:
  BallSpawnBlock(Scene& scene, const BallSpawnBlockDefinition& definition);
  BallSpawnBlock(Scene& scene, math::Vec2 p0, math::Vec2 p1);

  void onDestroy() final;
};

} // namespace blocks::game
