#include "game/Block.hpp"

#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/BlocksScene.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class BlockResourceSentinelData {
 public:
  BlockResourceSentinelData()
      : prototype_(
            GlobalSubSystemStack::get()
                .resourceManager()
                .loadResource<BlockPrototype>("Prototype_DefaultBlock")) {}

  engine::ResourceRef<BlockPrototype> getPrototype() { return prototype_; }

 private:
  engine::ResourceRef<BlockPrototype> prototype_;
};

constinit std::optional<BlockResourceSentinelData> resourceSentinel;

} // namespace

void BlockResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void BlockResourceSentinel::unload() {
  resourceSentinel.reset();
}

Block::Block(Scene& scene, const BlockDefinition& definition)
    : Block(
          scene,
          definition.position -
              definition.size.value_or(definition.prototype->size) / 2.f,
          definition.position +
              definition.size.value_or(definition.prototype->size) / 2.f,
          definition.prototype) {}

Block::Block(
    Scene& scene,
    math::Vec2 p0,
    math::Vec2 p1,
    engine::ResourceRef<BlockPrototype> prototype)
    : Actor(scene),
      physics::RectCollider(scene.getPhysicsScene(), p0, p1, 0b10, 0),
      Drawable(scene.getDrawableScene()),
      prototype_(prototype) {}

Block::Block(Scene& scene, math::Vec2 p0, math::Vec2 p1)
    : Block(scene, p0, p1, resourceSentinel->getPrototype()) {}

void Block::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  render.drawObject(
      window,
      0,
      prototype_->texture->get(),
      {math::modelMatrixFromBounds(getP0(), getP1())});
}

void Block::onDestroy() {
  Actor::onDestroy();
  BlocksScene* scene = dynamic_cast<BlocksScene*>(getScene());
  scene->onBlockDestroyed(*this);
}

} // namespace blocks::game
