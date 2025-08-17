#include "physics/RectCollider.hpp"

#include <cstdint>
#include <optional>
#include <utility>
#include "math/vec.hpp"
#include "physics/PhysicsScene.hpp"
#include "util/Registry.hpp"

namespace blocks::physics {

RectCollider::RectCollider(
    PhysicsScene& scene,
    math::Vec2 p0,
    math::Vec2 p1,
    uint64_t produceEventLayers,
    uint64_t receiveEventLayers)
    : RegistryItem(scene),
      produceEventsForLayers_(produceEventLayers),
      receiveEventsForLayers_(receiveEventLayers),
      p0_(p0),
      p1_(p1) {}

std::optional<math::Vec2> RectCollider::testCollision(
    const RectCollider& other) const {
  math::Vec2 overlapP0{
      std::max(p0_.x(), other.p0_.x()), std::max(p0_.y(), other.p0_.y())};
  math::Vec2 overlapP1{
      std::min(p1_.x(), other.p1_.x()), std::min(p1_.y(), other.p1_.y())};

  float xDiff = overlapP1.x() - overlapP0.x();
  float yDiff = overlapP1.y() - overlapP0.y();

  if (xDiff >= 0 && yDiff >= 0) {
    math::Vec2 centre = (p0_ + p1_) / 2.f;
    math::Vec2 otherCentre = (other.p0_ + other.p1_) / 2.f;
    if (xDiff > yDiff) {
      if (centre.y() < otherCentre.y()) {
        return math::Vec2{0.0f, -1.0f};
      } else {
        return math::Vec2{0.0f, 1.0f};
      }
    } else {
      if (centre.x() < otherCentre.x()) {
        return math::Vec2{-1.0f, 0.0f};
      } else {
        return math::Vec2{1.0f, 0.0f};
      }
    }
  }

  return std::nullopt;
}

} // namespace blocks::physics
