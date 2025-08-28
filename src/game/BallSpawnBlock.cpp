#include "game/BallSpawnBlock.hpp"

#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class BallSpawnBlockResourceSentinelData {
 public:
  BallSpawnBlockResourceSentinelData()
      : sprite_(GlobalSubSystemStack::get()
                    .renderSystem()
                    .createRenderable<render::RenderableTex2D>(
                        RESOURCE_DIR "/blockBallSpawn.png")) {}

  render::RenderableRef<render::RenderableTex2D::InstanceData> getSprite() {
    return sprite_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData> sprite_;
};

constinit std::optional<BallSpawnBlockResourceSentinelData> resourceSentinel;

} // namespace

void BallSpawnBlockResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void BallSpawnBlockResourceSentinel::unload() {
  resourceSentinel.reset();
}

BallSpawnBlock::BallSpawnBlock(Scene& scene, math::Vec2 p0, math::Vec2 p1)
    : Block(scene, p0, p1, resourceSentinel->getSprite()) {}

void BallSpawnBlock::onDestroy() {
  Block::onDestroy();
  getScene()->createActor<Ball>(
      (getP0() + getP1()) / 2.0f, math::Vec2{0.0f, 1.0f});
}

} // namespace blocks::game
