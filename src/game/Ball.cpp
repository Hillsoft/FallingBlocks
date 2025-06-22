#include "game/Ball.hpp"

#include <cstdlib>
#include "GlobalSubSystemStack.hpp"
#include "math/vec.hpp"

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
    : pos_(-kBallSize / 2.f, -kBallSize / 2.f),
      vel_(randFloat(-1.f, 1.f), randFloat(-1.f, 1.f)),
      sprite_(GlobalSubSystemStack::get().renderSystem().createRenderable(
          RESOURCE_DIR "/test_image.bmp")) {}

void Ball::update(float deltaTimeSeconds) {
  math::Vec2 objSize(kBallSize, kBallSize);
  math::Vec2 newPos = pos_ + deltaTimeSeconds * vel_;
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

  pos_ = newPos;
}

void Ball::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  sprite_->setPosition(pos_, pos_ + math::Vec2(kBallSize, kBallSize));
  render.drawObject(window, *sprite_);
}

} // namespace blocks::game
