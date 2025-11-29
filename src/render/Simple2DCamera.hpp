#pragma once

#include <vulkan/vulkan_core.h>
#include "math/vec.hpp"

namespace blocks::render {

class Simple2DCamera {
 public:
  enum AspectRatioHandling : uint8_t { FIT, FILL, STRETCH };
  struct Target {
    math::Vec2 centre;
    math::Vec2 extent;
  };

  Simple2DCamera(
      math::Vec2 worldP0,
      math::Vec2 worldP1,
      AspectRatioHandling ratioHandling);
  Simple2DCamera(Target target, AspectRatioHandling ratioHandling);

  [[nodiscard]] math::Mat3 getViewMatrix(VkExtent2D screenExtent) const;

  void setWorldViewRect(math::Vec2 p0, math::Vec2 p1);
  void setWorldTarget(Target target);
  void setRatioHandling(AspectRatioHandling ratioHandling);

 private:
  Target target_;
  AspectRatioHandling ratioHandling_;
};

} // namespace blocks::render
