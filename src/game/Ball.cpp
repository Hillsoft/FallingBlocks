#include "game/Ball.hpp"

#include <chrono>
#include <cstdint>
#include <optional>
#include <random>
#include "GlobalSubSystemStack.hpp"
#include "audio/AudioSubSystem.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "game/Block.hpp"
#include "game/BlocksScene.hpp"
#include "game/Paddle.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

namespace {

constexpr float kBallSize = 15.f * 0.05f;
constexpr float kBounceDirectionalStrength = 15.f;
constexpr float kBallSpeed = 15.f * 1.5f;

constexpr uint32_t kWallHitFrequency = 262;
constexpr uint32_t kBlockHitFrequency = 330;
constexpr uint32_t kPaddleHitFrequency = 392;
constexpr std::chrono::milliseconds kSoundDuration{20};

float randFloat(float lo, float hi) {
  static std::default_random_engine randomEngine{std::random_device{}()};
  std::uniform_real_distribution<float> distribution{lo, hi};
  return distribution(randomEngine);
}

math::Vec2 normalizeBallSpeed(const math::Vec2& speed) {
  return (kBallSpeed / speed.mag()) * speed;
}

math::Vec2 reflect(math::Vec2 vel, math::Vec2 normal) {
  return vel - 2.f * (vel.dot(normal)) * normal;
}

} // namespace

Ball::Ball(Scene& scene, const BallDefinition& definition)
    : Ball(
          scene,
          definition.prototype,
          definition.position.value_or(math::Vec2{15.0f, 15.0f}),
          definition.velocity.value_or(
              math::Vec2{randFloat(-0.25f, 0.25f), -1.0f})) {}

Ball::Ball(
    Scene& scene,
    engine::ResourceRef<BallPrototype> prototype,
    math::Vec2 pos,
    math::Vec2 vel)
    : Actor(scene),
      physics::RectCollider(
          scene.getPhysicsScene(),
          pos - math::Vec2{kBallSize, kBallSize} / 2.f,
          pos + math::Vec2{kBallSize, kBallSize} / 2.f,
          0,
          0b11),
      TickHandler(scene.getTickRegistry()),
      Drawable(scene.getDrawableScene()),
      prototype_(prototype),
      vel_(normalizeBallSpeed(vel)) {}

void Ball::update(float deltaTimeSeconds) {
  math::Vec2 objSize(kBallSize, kBallSize);
  math::Vec2 newPos = getP0() + deltaTimeSeconds * vel_;
  math::Vec2 newPos2 = newPos + objSize;

  if (newPos2.x() > 30.0f) {
    vel_.x() *= -1.0f;
    newPos.x() = 30.0f - objSize.x();
    GlobalSubSystemStack::get().audioSystem().playSineWave(
        audio::SineWave{
            .frequency = kWallHitFrequency,
            .volume = 0.5f,
            .duration = kSoundDuration});
  }
  if (newPos.x() < 0.0f) {
    vel_.x() *= -1.0f;
    newPos.x() = 0.0f;
    GlobalSubSystemStack::get().audioSystem().playSineWave(
        audio::SineWave{
            .frequency = kWallHitFrequency,
            .volume = 0.5f,
            .duration = kSoundDuration});
  }

  if (newPos.y() > 30.0f) {
    getScene()->destroyActor(this);
  }
  if (newPos.y() < 0.0f) {
    vel_.y() *= -1.0f;
    newPos.y() = 0.0f;
    GlobalSubSystemStack::get().audioSystem().playSineWave(
        audio::SineWave{
            .frequency = kWallHitFrequency,
            .volume = 0.5f,
            .duration = kSoundDuration});
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
      prototype_->texture->get(),
      {math::modelMatrixFromBounds(getP0(), getP1())});
}

void Ball::handleCollision(physics::RectCollider& other, math::Vec2 normal) {
  if (vel_.dot(normal) > 0) {
    return;
  }

  vel_ = reflect(vel_, normal);

  if (const auto* paddle = dynamic_cast<Paddle*>(&other); paddle != nullptr) {
    math::Vec2 ballCenter = (getP0() + getP1()) / 2.f;
    math::Vec2 paddleCenter = (paddle->getP0() + paddle->getP1()) / 2.f;

    vel_.x() = 0;
    vel_ = normalizeBallSpeed(vel_);

    vel_.x() = kBounceDirectionalStrength * (ballCenter.x() - paddleCenter.x());

    vel_ = normalizeBallSpeed(vel_);

    GlobalSubSystemStack::get().audioSystem().playSineWave(
        audio::SineWave{
            .frequency = kPaddleHitFrequency,
            .volume = 0.5f,
            .duration = kSoundDuration});
  } else if (auto* block = dynamic_cast<Block*>(&other); block != nullptr) {
    getScene()->destroyActor(block);

    GlobalSubSystemStack::get().audioSystem().playSineWave(
        audio::SineWave{
            .frequency = kBlockHitFrequency,
            .volume = 0.5f,
            .duration = kSoundDuration});
  }
}

void Ball::onDestroy() {
  auto* scene = dynamic_cast<BlocksScene*>(getScene());
  scene->onBallDestroyed(*this);
}

} // namespace blocks::game
