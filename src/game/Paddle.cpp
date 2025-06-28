#include "game/Paddle.hpp"

#include <utility>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"

namespace blocks::game {

namespace {

constexpr float kPaddleWidth = 0.3f;
constexpr float kPaddleHeight = 0.1f;

constexpr float kPaddleSpeed = 1.0f;

} // namespace

Paddle::Paddle()
    : input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      physics::RectCollider(
          GlobalSubSystemStack::get().physicsScene(),
          math::Vec2{-kPaddleWidth / 2.f, 0.8f},
          math::Vec2{kPaddleWidth / 2.f, 0.9f},
          0b1,
          0),
      TickHandler(GlobalSubSystemStack::get().tickSystem()),
      Drawable(GlobalSubSystemStack::get().drawableScene()),
      sprite_(GlobalSubSystemStack::get().renderSystem().createRenderable(
          RESOURCE_DIR "/mandelbrot set.png")) {}

void Paddle::update(float deltaTimeSeconds) {
  math::Vec2 pos = getP0();
  pos += deltaTimeSeconds * vel_;

  pos.x() = std::min(1.0f - kPaddleWidth, pos.x());
  pos.x() = std::max(-1.0f, pos.x());

  setP0(pos);
  setP1(pos + math::Vec2{kPaddleWidth, kPaddleHeight});
}

void Paddle::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  sprite_->setPosition(getP0(), getP1());
  render.drawObject(window, *sprite_);
}

void Paddle::onKeyPress(int key) {
  if (key == GLFW_KEY_RIGHT) {
    vel_ = math::Vec2(kPaddleSpeed, 0.0f);
  } else if (key == GLFW_KEY_LEFT) {
    vel_ = math::Vec2(-kPaddleSpeed, 0.0f);
  }
}

void Paddle::onKeyRelease(int key) {
  if (key == GLFW_KEY_RIGHT && vel_.x() > 0.0f) {
    vel_ = 0.0f;
  } else if (key == GLFW_KEY_LEFT && vel_.x() < 0.0f) {
    vel_ = 0.0f;
  }
}

} // namespace blocks::game
