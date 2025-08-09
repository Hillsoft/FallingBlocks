#pragma once

#include <GLFW/glfw3.h>
#include "math/vec.hpp"

namespace blocks::render {

class Simple2DCamera {
 public:
  enum AspectRatioHandling { FIT, FILL, STRETCH };
  struct Target {
    math::Vec2 centre;
    math::Vec2 extent;
  };

  Simple2DCamera(
      math::Vec2 worldP0,
      math::Vec2 worldP1,
      AspectRatioHandling ratioHandling);
  Simple2DCamera(Target target, AspectRatioHandling ratioHandling);

  math::Mat3 getViewMatrix(VkExtent2D screenExtent) const;

  void setWorldViewRect(math::Vec2 p0, math::Vec2 p1);
  void setWorldTarget(Target target);
  void setRatioHandling(AspectRatioHandling ratioHandling);

 private:
  Target target_;
  AspectRatioHandling ratioHandling_;
};

} // namespace blocks::render
