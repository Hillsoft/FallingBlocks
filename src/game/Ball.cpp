#include "game/Ball.hpp"

#include <cstdlib>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "game/Block.hpp"
#include "game/Paddle.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/renderables/RenderableTex2D.hpp"

namespace blocks::game {

namespace {

constexpr float kBallSize = 0.05f;
constexpr float kBounceDirectionalStrength = 15.f;
constexpr float kBallSpeed = 1.5f;

float randFloat(float lo, float hi) {
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  r *= (hi - lo);
  r += lo;
  return r;
}

math::Vec2 normalizeBallSpeed(const math::Vec2& speed) {
  return (kBallSpeed / speed.mag()) * speed;
}

} // namespace

Ball::Ball(Scene& scene)
    : Actor(scene),
      physics::RectCollider(
          scene.getPhysicsScene(),
          math::Vec2{-kBallSize / 2.f, -kBallSize / 2.f},
          math::Vec2{kBallSize / 2.f, kBallSize / 2.f},
          0,
          0b11),
      TickHandler(scene.getTickRegistry()),
      Drawable(scene.getDrawableScene()),
      vel_(normalizeBallSpeed(math::Vec2{randFloat(-0.25f, 0.25f), -1.0f})),
      sprite_(GlobalSubSystemStack::get()
                  .renderSystem()
                  .createRenderable<render::RenderableTex2D>(
                      RESOURCE_DIR "/ball.png")) {}

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

  if (newPos.y() > 1.0f) {
    getScene()->destroyActor(this);
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

  render.drawObject(
      window, *sprite_, {math::modelMatrixFromBounds(getP0(), getP1())});
}

void Ball::handleCollision(physics::RectCollider& other) {
  if (auto paddle = dynamic_cast<Paddle*>(&other); paddle != nullptr) {
    vel_.y() = -std::abs(vel_.y());

    math::Vec2 ballCenter = (getP0() + getP1()) / 2.f;
    math::Vec2 paddleCenter = (paddle->getP0() + paddle->getP1()) / 2.f;

    vel_.x() = 0;
    vel_ = normalizeBallSpeed(vel_);

    vel_.x() = kBounceDirectionalStrength * (ballCenter.x() - paddleCenter.x());

    vel_ = normalizeBallSpeed(vel_);
  } else if (auto block = dynamic_cast<Block*>(&other); block != nullptr) {
    vel_.y() = std::abs(vel_.y());
    getScene()->destroyActor(block);
  }
}

} // namespace blocks::game
