#include "game/Ball.hpp"

#include <cstdlib>
#include "GlobalSubSystemStack.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"

namespace blocks::game {

namespace {

constexpr float kBallSize = 0.05f;

float randFloat(float lo, float hi) {
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  r *= (hi - lo);
  r += lo;
  return r;
}

} // namespace

Ball::Ball()
    : physics::RectCollider(
          GlobalSubSystemStack::get().physicsScene(),
          math::Vec2{-kBallSize / 2.f, -kBallSize / 2.f},
          math::Vec2{kBallSize / 2.f, kBallSize / 2.f}),
      TickHandler(GlobalSubSystemStack::get().tickSystem()),
      Drawable(GlobalSubSystemStack::get().drawableScene()),
      vel_(randFloat(-1.f, 1.f), randFloat(-1.f, 1.f)),
      sprite_(GlobalSubSystemStack::get().renderSystem().createRenderable(
          RESOURCE_DIR "/test_image.bmp")) {}

void Ball::update(float deltaTimeSeconds) {
  math::Vec2 objSize(kBallSize, kBallSize);
  math::Vec2 newPos = getP0() + deltaTimeSeconds * vel_;
  math::Vec2 newPos2 = newPos + objSize;

  if (newPos2.x() > 1.0f) {
    vel_.x() *= -1.0f;
    newPos.x() = 1.0f - objSize.x();
  }
  if (newPos.x() < -1.0f) {
    vel_.x() *= -1.0f;
    newPos.x() = -1.0f;
  }

  if (newPos2.y() > 1.0f) {
    vel_.y() *= -1.0f;
    newPos.y() = 1.0f - objSize.y();
  }
  if (newPos.y() < -1.0f) {
    vel_.y() *= -1.0f;
    newPos.y() = -1.0f;
  }

  setP0(newPos);
  setP1(newPos2);
}

void Ball::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  sprite_->setPosition(getP0(), getP1());
  render.drawObject(window, *sprite_);
}

void Ball::handleCollision(physics::RectCollider& other) {
  vel_.y() = -std::abs(vel_.y());
}

} // namespace blocks::game
