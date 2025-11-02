#include "game/StaticImage.hpp"

#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

StaticImage::StaticImage(Scene& scene, const StaticImageDefinition& definition)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      prototype_(definition.prototype),
      minPos_(
          definition.position -
          definition.size.value_or(prototype_->size) / 2.f),
      maxPos_(
          definition.position +
          definition.size.value_or(prototype_->size) / 2.f),
      z_(definition.z.value_or(prototype_->z)) {}

void StaticImage::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  render.drawObject(
      window,
      -100,
      prototype_->texture->get(),
      {math::modelMatrixFromBounds(
          math::Vec2{-1.0f, -1.0f}, math::Vec2{1.0f, 1.0f})});
}

} // namespace blocks::game
