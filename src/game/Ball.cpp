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
constexpr float kBounceDirectionalStrength = 5.f;

float randFloat(float lo, float hi) {
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  r *= (hi - lo);
  r += lo;
  return r;
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
      vel_(randFloat(-1.f, 1.f), randFloat(-1.f, 1.f)),
      sprite_(GlobalSubSystemStack::get()
                  .renderSystem()
                  .createRenderable<render::RenderableTex2D>(
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

  render.drawObject(
      window, *sprite_, {math::modelMatrixFromBounds(getP0(), getP1())});
}

void Ball::handleCollision(physics::RectCollider& other) {
  if (auto paddle = dynamic_cast<Paddle*>(&other); paddle != nullptr) {
    vel_.y() = -std::abs(vel_.y());

    math::Vec2 ballCenter = (getP0() + getP1()) / 2.f;
    math::Vec2 paddleCenter = (paddle->getP0() + paddle->getP1()) / 2.f;

    vel_.x() = kBounceDirectionalStrength * (ballCenter.x() - paddleCenter.x());

  } else if (auto block = dynamic_cast<Block*>(&other); block != nullptr) {
    vel_.y() = std::abs(vel_.y());
    getScene()->destroyActor(block);
  }
}

} // namespace blocks::game
