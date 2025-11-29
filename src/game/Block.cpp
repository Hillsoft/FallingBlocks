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

namespace blocks::game {

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
  auto* scene = dynamic_cast<BlocksScene*>(getScene());
  scene->onBlockDestroyed(*this);
}

} // namespace blocks::game
