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

class BallSpawnBlock : public Block {
 public:
  BallSpawnBlock(Scene& scene, math::Vec2 p0, math::Vec2 p1);

  void onDestroy() final;
};

} // namespace blocks::game
