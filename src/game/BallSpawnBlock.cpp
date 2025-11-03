#include "game/BallSpawnBlock.hpp"

#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "math/vec.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class BallSpawnBlockResourceSentinelData {
 public:
  BallSpawnBlockResourceSentinelData()
      : prototype_(
            GlobalSubSystemStack::get()
                .resourceManager()
                .loadResource<BlockPrototype>(
                    "Prototype_DefaultBallSpawnBlock")) {}

  engine::ResourceRef<BlockPrototype> getPrototype() { return prototype_; }

 private:
  engine::ResourceRef<BlockPrototype> prototype_;
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
