#include "game/Paddle.hpp"

#include <algorithm>
#include <optional>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

constexpr float kPaddleWidth = 0.3f;
constexpr float kPaddleHeight = 0.1f;

constexpr float kPaddleSpeed = 1.0f;

class PaddleResourceSentinelData {
 public:
  PaddleResourceSentinelData()
      : sprite_(GlobalSubSystemStack::get()
                    .renderSystem()
                    .createRenderable<render::RenderableTex2D>(
                        RESOURCE_DIR "/paddle.png")) {}

  render::RenderableRef<render::RenderableTex2D::InstanceData> getSprite() {
    return sprite_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData> sprite_;
};

constinit std::optional<PaddleResourceSentinelData> resourceSentinel;

} // namespace

void PaddleResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void PaddleResourceSentinel::unload() {
  resourceSentinel.reset();
}

Paddle::Paddle(Scene& scene)
    : Actor(scene),
      input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      physics::RectCollider(
          scene.getPhysicsScene(),
          math::Vec2{-kPaddleWidth / 2.f, 0.8f},
          math::Vec2{kPaddleWidth / 2.f, 0.9f},
          0b1,
          0),
      TickHandler(scene.getTickRegistry()),
      Drawable(scene.getDrawableScene()) {}

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

  render.drawObject(
      window,
      resourceSentinel->getSprite(),
      {math::modelMatrixFromBounds(getP0(), getP1())});
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
