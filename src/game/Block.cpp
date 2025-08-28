#include "game/Block.hpp"

#include <optional>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "game/BlocksScene.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class BlockResourceSentinelData {
 public:
  BlockResourceSentinelData()
      : sprite_(GlobalSubSystemStack::get()
                    .renderSystem()
                    .createRenderable<render::RenderableTex2D>(
                        RESOURCE_DIR "/block.png")) {}

  render::RenderableRef<render::RenderableTex2D::InstanceData> getSprite() {
    return sprite_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData> sprite_;
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

Block::Block(
    Scene& scene,
    math::Vec2 p0,
    math::Vec2 p1,
    render::RenderableRef<render::RenderableTex2D::InstanceData> sprite)
    : Actor(scene),
      physics::RectCollider(scene.getPhysicsScene(), p0, p1, 0b10, 0),
      Drawable(scene.getDrawableScene()),
      sprite_(std::move(sprite)) {}

Block::Block(Scene& scene, math::Vec2 p0, math::Vec2 p1)
    : Block(scene, p0, p1, resourceSentinel->getSprite()) {}

void Block::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  render.drawObject(
      window, sprite_, {math::modelMatrixFromBounds(getP0(), getP1())});
}

void Block::onDestroy() {
  Actor::onDestroy();
  BlocksScene* scene = dynamic_cast<BlocksScene*>(getScene());
  scene->onBlockDestroyed();
}

} // namespace blocks::game
