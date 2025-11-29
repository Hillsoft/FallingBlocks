#pragma once

#include <cstdint>
#include <optional>
#include "math/vec.hpp"
#include "physics/PhysicsScene.hpp"
#include "util/Registry.hpp"

namespace blocks::physics {

class RectCollider : public util::RegistryItem<PhysicsScene, RectCollider> {
 public:
  RectCollider(
      PhysicsScene& scene,
      math::Vec2 p0,
      math::Vec2 p1,
      uint64_t produceEventLayers = ~0ull,
      uint64_t receiveEventLayers = ~0ull);
  virtual ~RectCollider() = default;

  RectCollider(const RectCollider& other) = delete;
  RectCollider& operator=(const RectCollider& other) = delete;

  RectCollider(RectCollider&& other) = delete;
  RectCollider& operator=(RectCollider&& other) = delete;

  [[nodiscard]] std::optional<math::Vec2> testCollision(
      const RectCollider& other) const;

  virtual void handleCollision(RectCollider& other, math::Vec2 normal) {}

  [[nodiscard]] math::Vec2 getP0() const { return p0_; }
  void setP0(math::Vec2 newP0) { p0_ = newP0; }

  [[nodiscard]] math::Vec2 getP1() const { return p1_; }
  void setP1(math::Vec2 newP1) { p1_ = newP1; }

  [[nodiscard]] uint64_t getProduceEventLayers() const {
    return produceEventsForLayers_;
  }
  [[nodiscard]] uint64_t getReceiveEventLayers() const {
    return receiveEventsForLayers_;
  }

 private:
  uint64_t produceEventsForLayers_;
  uint64_t receiveEventsForLayers_;
  math::Vec2 p0_;
  math::Vec2 p1_;
};

} // namespace blocks::physics
