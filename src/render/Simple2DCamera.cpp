#include "render/Simple2DCamera.hpp"

#include <GLFW/glfw3.h>
#include "math/vec.hpp"

namespace blocks::render {

namespace {

Simple2DCamera::Target targetFromBounds(math::Vec2 p0, math::Vec2 p1) {
  return {(p0 + p1) / 2.0f, p1 - p0};
}

math::Vec2 getAspectScaledWorldExtent(
    math::Vec2 worldExtent,
    VkExtent2D viewExtent,
    Simple2DCamera::AspectRatioHandling handlingMethod) {
  float worldAspect = worldExtent.x() / worldExtent.y();

  float viewAspect = static_cast<float>(viewExtent.width) /
      static_cast<float>(viewExtent.height);

  switch (handlingMethod) {
    case Simple2DCamera::AspectRatioHandling::STRETCH:
      break;

    case Simple2DCamera::FIT:
      if (viewAspect > worldAspect) {
        worldExtent.x() *= viewAspect / worldAspect;
      } else {
        worldExtent.y() *= worldAspect / viewAspect;
      }
      break;

    case Simple2DCamera::FILL:
      if (viewAspect < worldAspect) {
        worldExtent.x() *= viewAspect / worldAspect;
      } else {
        worldExtent.y() *= worldAspect / viewAspect;
      }
      break;
  }

  return worldExtent;
}

} // namespace

Simple2DCamera::Simple2DCamera(
    math::Vec2 worldP0, math::Vec2 worldP1, AspectRatioHandling ratioHandling)
    : target_(targetFromBounds(worldP0, worldP1)),
      ratioHandling_(ratioHandling) {}

Simple2DCamera::Simple2DCamera(Target target, AspectRatioHandling ratioHandling)
    : target_(target), ratioHandling_(ratioHandling) {}

math::Mat3 Simple2DCamera::getViewMatrix(VkExtent2D screenExtent) const {
  math::Vec2 adjustedExtent =
      getAspectScaledWorldExtent(target_.extent, screenExtent, ratioHandling_);

  return math::Mat3::translate(-1.0f * target_.centre) *
      math::Mat3::scale(math::Vec2{
          2.0f / adjustedExtent.x(), 2.0f / adjustedExtent.y()});
}

void Simple2DCamera::setWorldViewRect(math::Vec2 p0, math::Vec2 p1) {
  target_ = targetFromBounds(p0, p1);
}

void Simple2DCamera::setWorldTarget(Target target) {
  target_ = target;
}

void Simple2DCamera::setRatioHandling(AspectRatioHandling ratioHandling) {
  ratioHandling_ = ratioHandling;
}

} // namespace blocks::render
