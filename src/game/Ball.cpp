#include "game/Ball.hpp"

#include <cstdlib>
#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "game/Block.hpp"
#include "game/BlocksScene.hpp"
#include "game/Paddle.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/debug.hpp"

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

math::Vec2 reflect(math::Vec2 vel, math::Vec2 normal) {
  return vel - 2.f * (vel.dot(normal)) * normal;
}

class BallResourceSentinelData {
 public:
  BallResourceSentinelData()
      : sprite_(GlobalSubSystemStack::get()
                    .renderSystem()
                    .createRenderable<render::RenderableTex2D>(
                        RESOURCE_DIR "/ball.png")) {}

  render::RenderableRef<render::RenderableTex2D::InstanceData> getSprite() {
    return sprite_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData> sprite_;
};

constinit std::optional<BallResourceSentinelData> resourceSentinel;

} // namespace

void BallResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void BallResourceSentinel::unload() {
  resourceSentinel.reset();
}

Ball::Ball(Scene& scene, math::Vec2 pos, math::Vec2 vel)
    : Actor(scene),
      physics::RectCollider(
          scene.getPhysicsScene(),
          pos - math::Vec2{-kBallSize, -kBallSize} / 2.f,
          pos + math::Vec2{-kBallSize, -kBallSize} / 2.f,
          0,
          0b11),
      TickHandler(scene.getTickRegistry()),
      Drawable(scene.getDrawableScene()),
      vel_(normalizeBallSpeed(vel)) {}

Ball::Ball(Scene& scene)
    : Ball(
          scene,
          math::Vec2{0.0f},
          math::Vec2{randFloat(-0.25f, 0.25f), -1.0f}) {}

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
      window,
      0,
      resourceSentinel->getSprite(),
      {math::modelMatrixFromBounds(getP0(), getP1())});
}

void Ball::handleCollision(physics::RectCollider& other, math::Vec2 normal) {
  if (vel_.dot(normal) > 0) {
    return;
  }

  vel_ = reflect(vel_, normal);

  if (auto paddle = dynamic_cast<Paddle*>(&other); paddle != nullptr) {
    math::Vec2 ballCenter = (getP0() + getP1()) / 2.f;
    math::Vec2 paddleCenter = (paddle->getP0() + paddle->getP1()) / 2.f;

    vel_.x() = 0;
    vel_ = normalizeBallSpeed(vel_);

    vel_.x() = kBounceDirectionalStrength * (ballCenter.x() - paddleCenter.x());

    vel_ = normalizeBallSpeed(vel_);
  } else if (auto block = dynamic_cast<Block*>(&other); block != nullptr) {
    getScene()->destroyActor(block);
  }
}

void Ball::onDestroy() {
  BlocksScene* scene = dynamic_cast<BlocksScene*>(getScene());
  scene->onBallDestroyed(*this);
}

} // namespace blocks::game
