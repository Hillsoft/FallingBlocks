#include "physics/RectCollider.hpp"

#include <cstdint>
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

bool RectCollider::testCollision(const RectCollider& other) const {
  math::Vec2 overlapP0{
      std::max(p0_.x(), other.p0_.x()), std::max(p0_.y(), other.p0_.y())};
  math::Vec2 overlapP1{
      std::min(p1_.x(), other.p1_.x()), std::min(p1_.y(), other.p1_.y())};

  return overlapP0.x() <= overlapP1.x() && overlapP0.y() <= overlapP1.y();
}

} // namespace blocks::physics
