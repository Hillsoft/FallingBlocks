#include "game/BallSpawnBlock.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "math/vec.hpp"
#include "render/renderables/RenderableTex2D.hpp"

namespace blocks::game {

BallSpawnBlock::BallSpawnBlock(Scene& scene, math::Vec2 p0, math::Vec2 p1)
    : Block(
          scene,
          p0,
          p1,
          GlobalSubSystemStack::get()
              .renderSystem()
              .createRenderable<render::RenderableTex2D>(
                  RESOURCE_DIR "/blockBallSpawn.png")) {}

void BallSpawnBlock::onDestroy() {
  Block::onDestroy();
  getScene()->createActor<Ball>(
      (getP0() + getP1()) / 2.0f, math::Vec2{0.0f, 1.0f});
}

} // namespace blocks::game
