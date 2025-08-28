#include "game/Background.hpp"

#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class BackgroundResourceSentinelData {
 public:
  BackgroundResourceSentinelData()
      : sprite_(GlobalSubSystemStack::get()
                    .renderSystem()
                    .createRenderable<render::RenderableTex2D>(
                        RESOURCE_DIR "/background.png")) {}

  render::RenderableRef<render::RenderableTex2D::InstanceData> getSprite() {
    return sprite_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData> sprite_;
};

constinit std::optional<BackgroundResourceSentinelData> resourceSentinel;

} // namespace

void BackgroundResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void BackgroundResourceSentinel::unload() {
  resourceSentinel.reset();
}

Background::Background(Scene& scene)
    : Actor(scene), Drawable(scene.getDrawableScene()) {}

void Background::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  render.drawObject(
      window,
      resourceSentinel->getSprite(),
      {math::modelMatrixFromBounds(
          math::Vec2{-1.0f, -1.0f}, math::Vec2{1.0f, 1.0f})});
}

} // namespace blocks::game
