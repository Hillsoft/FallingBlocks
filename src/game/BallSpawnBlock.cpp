#include "game/BallSpawnBlock.hpp"

#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "math/vec.hpp"

namespace blocks::game {

BallSpawnBlock::BallSpawnBlock(
    Scene& scene, const BallSpawnBlockDefinition& definition)
    : Block(scene, definition),
      ballPrototype_(definition.prototype->ballPrototype) {}

void BallSpawnBlock::onDestroy() {
  Block::onDestroy();
  getScene()->createActor<Ball>(
      ballPrototype_, (getP0() + getP1()) / 2.0f, math::Vec2{0.0f, 1.0f});
}

} // namespace blocks::game
